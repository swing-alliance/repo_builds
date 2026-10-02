import os
# 【核心修复】必须在导入 akshare、pandas 等包含 C 扩展的库之前设置此环境变量，解决 Python 3.13 内存池崩溃问题
os.environ["PYTHONMALLOC"] = "malloc"
import datetime
import time
import akshare as ak
import pandas as pd
from typing import List, Optional,Dict
from concurrent.futures import ThreadPoolExecutor, wait, FIRST_COMPLETED
from py_obj import FundInst
from db_obj import FundInfo
import math
import json
from diskio.dbio import get_fund_info_db



def instf_netio_fund(code: str) -> Optional[FundInst]:
    """
    akshare 拉取基金累计净值（成立到今日），封装返回 FundInst 对象
    :param code: 基金代码，如 "110020"
    :return: FundInst，拉取失败返回 None
    """
    try:
        # 适当的微小停顿，避免过快并发触发东财限制或底层网络连接池异常
        time.sleep(0.1)
        df = ak.fund_open_fund_info_em(symbol=code, indicator="累计净值走势")
        if df is None or df.empty:
            print(f"❌基金{code}获取数据为空")
            return None
            
        df["净值日期"] = pd.to_datetime(df["净值日期"])
        df = df.sort_values("净值日期").reset_index(drop=True)
        df["daily_return"] = df["累计净值"].pct_change(periods=1).round(6)
        pack = FundInst(code=code, df=df)
        print(f"✅基金{code}获取成功，共{len(df)}条累计净值记录")
        return pack
    except Exception as e:
        print(f"❌基金{code}拉取失败: {e}")
        return None

def multi_instf_netio_fund(code_list: List[str], max_workers: int = 2, stop_flag=None) -> List[Optional[FundInst]]:
    """
    【安全版】手动管理线程池，移除with，支持外部停止标记，取消未执行任务
    :param code_list: 基金代码列表 ["110020","001984"]
    :param max_workers: 线程数量，推荐 2，不要超过3，容易触发东财限流
    :param stop_flag: threading.Event() 外部停止信号
    :return: List[FundInst | None]，顺序和输入code_list一一对应，失败项为None
    """
    total = len(code_list)
    result: List[Optional[FundInst]] = [None] * total
    future_map = {}
    executor = ThreadPoolExecutor(max_workers=max_workers)
    try:
        # 提交任务
        for idx, code in enumerate(code_list):
            if stop_flag and stop_flag.is_set():
                break
            fut = executor.submit(instf_netio_fund, code)
            future_map[fut] = idx

        remaining = set(future_map.keys())
        while remaining:
            if stop_flag and stop_flag.is_set():
                break
            done, remaining = wait(
                remaining,
                timeout=0.5,
                return_when=FIRST_COMPLETED
            )
            for fut in done:
                idx = future_map[fut]
                try:
                    res = fut.result(timeout=20)
                    result[idx] = res
                except Exception as e:
                    print(f"❌任务异常 code={code_list[idx]} : {e}")
                    result[idx] = None
    finally:
        # 核心：取消等待中的future，不卡死等待僵死网络请求
        executor.shutdown(wait=False, cancel_futures=True)
    return result



def fetch_fund_latest_info(fund_code: str) -> pd.DataFrame:
    """
    输入6位基金代码，拉取基金基础信息并在终端打印df
    :param fund_code: 6位字符串，例 "000001"
    """
    # 输入校验：6位纯数字
    if not (len(fund_code) == 6 and fund_code.isdigit()):
        print("[ERROR] 请输入6位数字基金代码，格式示例：000001")
        return
    try:
        df: pd.DataFrame = ak.fund_overview_em(symbol=fund_code)
        row = df.iloc[0]
        fund_short_name = row.get("基金简称", "")
        fund_type = row.get("基金类型", "")
        # ========== 重点修改 ==========
        # 不要用row.get("基金代码")，改用入参fund_code（干净6位数字）
        f_code = fund_code
        raw_df_str = df.to_string()  # 完整原始df转字符串
        df_fund_info = pd.DataFrame([{
            "基金代码": f_code,
            "基金简称": fund_short_name,
            "基金类型": fund_type,
            "基金信息": raw_df_str
        }])
        return df_fund_info
    except Exception as err:
        print(f"[ERROR] 获取基金数据失败：{str(err)}")

def fetch_fund_latest_info_batch_thread(code_list: List[str], max_workers: int = 2) -> List[Optional[pd.DataFrame]]:
    """
    批量并发拉取基金基础概览信息，移除stop_flag，风格对齐净值多线程代码
    :param code_list: 基金代码列表
    :param max_workers: 线程数，建议 1~2
    :return: List[pd.DataFrame | None] 和输入顺序保持一致
    """
    total = len(code_list)
    print(f"🌐 开始批量拉取基金概览，共 {total} 只，线程数 {max_workers}")  # 新增
    result: List[Optional[pd.DataFrame]] = [None] * total
    future_map = {}
    executor = ThreadPoolExecutor(max_workers=max_workers)
    try:
        for idx, code in enumerate(code_list):
            fut = executor.submit(fetch_fund_latest_info, code)
            future_map[fut] = idx
        print(f"✅ 全部任务提交完成，等待网络返回...")  # 新增
        completed_cnt = 0
        remaining = set(future_map.keys())
        while remaining:
            done, remaining = wait(
                remaining,
                timeout=0.5,
                return_when=FIRST_COMPLETED
            )
            for fut in done:
                idx = future_map[fut]
                try:
                    res = fut.result(timeout=20)
                    result[idx] = res
                except Exception as e:
                    print(f"❌概览任务异常 code={code_list[idx]} : {e}")
                    result[idx] = None
                completed_cnt += 1
            # 每轮打印一次进度，不用每条都打
            if done:
                print(f"⏳ 进度：{completed_cnt}/{total}")  # 新增
        # 最终统计
        success_cnt = sum(1 for x in result if x is not None)
        print(f"🏁 本批拉取完成：成功 {success_cnt} 只，失败 {total - success_cnt} 只")  # 新增 
    finally:
        executor.shutdown(wait=False, cancel_futures=True)
    return result





def fetch_fund_latest_holdings_json_batch_thread(
    code_list: List[str],
    max_workers: int = 2,
    years_back: int = 3,
    top_n: int = 50,
) -> Dict[str, str]:
    """
    批量并发拉取基金最新季度持仓 JSON。
    返回：{fund_code: json_str}，只包含成功拉取到的基金。
    成功为 JSON 字符串（可能为 "[]"），失败直接丢弃。
    """
    def _fetch_one(fund_code: str) -> str:
        """单只：拉最新季度持仓，返回 JSON 字符串"""
        now_year = datetime.datetime.now().year
        frames = []
        for y in range(now_year, now_year - years_back, -1):
            try:
                df = ak.fund_portfolio_hold_em(symbol=fund_code, date=str(y))
                if df is not None and not df.empty:
                    frames.append(df)
            except Exception as e:
                print(f"⚠️ {fund_code} {y} 年拉取失败: {e}")
                continue
        if not frames:
            return "[]"
        all_df = pd.concat(frames, ignore_index=True)
        def _parse_quarter(q: str):
            q = str(q).replace("股票投资明细", "").strip()
            try:
                year_part, rest = q.split("年")
                return int(year_part), int(rest.replace("季度", ""))
            except Exception:
                return (0, 0)
        parsed = all_df["季度"].map(_parse_quarter)
        all_df["_year"] = [p[0] for p in parsed]
        all_df["_quarter"] = [p[1] for p in parsed]

        latest = (
            all_df[["_year", "_quarter"]]
            .drop_duplicates()
            .sort_values(["_year", "_quarter"], ascending=False)
            .iloc[0]
        )
        latest_year, latest_q = int(latest["_year"]), int(latest["_quarter"])
        result = all_df[
            (all_df["_year"] == latest_year) & (all_df["_quarter"] == latest_q)
        ].copy()
        if "持仓市值" in result.columns:
            result = result.sort_values("持仓市值", ascending=False)
        if top_n and top_n > 0:
            result = result.head(top_n)
        def _safe_float(v) -> float:
            try:
                f = float(v)
                return 0.0 if math.isnan(f) or math.isinf(f) else f
            except (TypeError, ValueError):
                return 0.0
        records = []
        for _, row in result.iterrows():
            records.append({
                "code":    str(row["股票代码"]).zfill(6),
                "name":    str(row["股票名称"]),
                "ratio":   _safe_float(row["占净值比例"]),
                "shares":  _safe_float(row["持股数"]),
                "value":   _safe_float(row["持仓市值"]),
                "quarter": str(row["季度"]).replace("股票投资明细", ""),
            })
        return json.dumps(records, ensure_ascii=False)
    # ---------- 批量调度 ----------
    total = len(code_list)
    print(f"🌐 开始批量拉取基金最新持仓，共 {total} 只，线程数 {max_workers}")
    result: Dict[str, str] = {}
    future_map = {}
    executor = ThreadPoolExecutor(max_workers=max_workers)
    try:
        for code in code_list:
            fut = executor.submit(_fetch_one, code)
            future_map[fut] = code
        print("✅ 全部任务提交完成，等待网络返回...")
        completed_cnt = 0
        remaining = set(future_map.keys())
        while remaining:
            done, remaining = wait(
                remaining,
                timeout=0.5,
                return_when=FIRST_COMPLETED,
            )
            for fut in done:
                code = future_map[fut]
                try:
                    res = fut.result(timeout=20)
                    if res is not None:
                        result[code] = res
                except Exception as e:
                    print(f"❌持仓任务异常 code={code} : {e}")
                completed_cnt += 1

            if done:
                print(f"⏳ 进度：{completed_cnt}/{total}")
        success_cnt = len(result)
        print(f"🏁 本批拉取完成：成功 {success_cnt} 只，失败 {total - success_cnt} 只")
    finally:
        executor.shutdown(wait=False, cancel_futures=True)
    return result


