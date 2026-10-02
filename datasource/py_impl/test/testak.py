import os
os.environ["HTTP_PROXY"] = ""
os.environ["HTTPS_PROXY"] = ""
os.environ["ALL_PROXY"] = ""
os.environ["all_proxy"] = ""
os.environ["NO_PROXY"] = "*"

import json
import math
import datetime
import requests
import akshare as ak
import pandas as pd

# ---------- 全局超时补丁 ----------
_orig_request = requests.Session.request

def _patched_request(self, method, url, **kwargs):
    kwargs.setdefault("timeout", 15)
    return _orig_request(self, method, url, **kwargs)

requests.Session.request = _patched_request
# ---------------------------------


def _safe_float(v) -> float:
    """NaN / Inf / None / 非数字 统一转 0.0，保证 JSON 合法"""
    try:
        f = float(v)
        return 0.0 if math.isnan(f) or math.isinf(f) else f
    except (TypeError, ValueError):
        return 0.0


def _parse_quarter(q: str):
    """'2026年2季度股票投资明细' -> (2026, 2)"""
    q = str(q).replace("股票投资明细", "").strip()
    try:
        year_part, rest = q.split("年")
        return int(year_part), int(rest.replace("季度", ""))
    except Exception:
        return (0, 0)


def get_all_holdings(fund_code: str, years_back: int = 3) -> pd.DataFrame:
    """从今年往前翻 years_back 年，拼接所有季度数据"""
    now_year = datetime.datetime.now().year
    frames = []
    for y in range(now_year, now_year - years_back, -1):
        try:
            df = ak.fund_portfolio_hold_em(symbol=fund_code, date=str(y))
            if df is not None and not df.empty:
                frames.append(df)
        except Exception as e:
            print(f"{y} 年获取失败: {e}", file=__import__("sys").stderr)
    if not frames:
        return pd.DataFrame()
    return pd.concat(frames, ignore_index=True)


def get_latest_quarter(fund_code: str, years_back: int = 3) -> pd.DataFrame:
    """只返回最新一个季度的持仓明细"""
    all_df = get_all_holdings(fund_code, years_back)
    if all_df.empty:
        return all_df

    all_df["_year"], all_df["_quarter"] = zip(*all_df["季度"].map(_parse_quarter))

    latest = all_df[["_year", "_quarter"]].drop_duplicates().sort_values(
        ["_year", "_quarter"], ascending=False
    ).iloc[0]
    latest_year, latest_q = int(latest["_year"]), int(latest["_quarter"])

    result = all_df[(all_df["_year"] == latest_year) & (all_df["_quarter"] == latest_q)].copy()
    result = result.drop(columns=["_year", "_quarter"])

    if "持仓市值" in result.columns:
        result = result.sort_values("持仓市值", ascending=False)

    return result.reset_index(drop=True)


def get_stocks_info_json(fund_code: str, top_n: int = 50, years_back: int = 3) -> str:
    """
    输入基金代码，返回可直接写入 FundInfoFlex.stocks_info 的 JSON 字符串。
    结构：[{"code","name","ratio","shares","value","quarter"}, ...]
    - code:    6 位股票代码（字符串，保留前导零）
    - name:    股票名称
    - ratio:   占净值比例（%）
    - shares:  持股数（万股）
    - value:   持仓市值（万元）
    - quarter: 季度标识，如 "2026年2季度"
    """
    df = get_latest_quarter(fund_code, years_back)
    if df is None or df.empty:
        return "[]"

    # 只保留前 top_n 大
    df = df.head(top_n)

    records = []
    for _, row in df.iterrows():
        records.append({
            "code":    str(row["股票代码"]).zfill(6),
            "name":    str(row["股票名称"]),
            "ratio":   _safe_float(row["占净值比例"]),
            "shares":  _safe_float(row["持股数"]),
            "value":   _safe_float(row["持仓市值"]),
            "quarter": str(row["季度"]).replace("股票投资明细", ""),
        })

    return json.dumps(records, ensure_ascii=False)


# ---------- 可选：直接落库的辅助函数 ----------
def save_to_db(session, FundInfoFlex, fund_code: str, top_n: int = 50):
    """把最新持仓写入 FundInfoFlex.stocks_info 并刷新 update_time"""
    json_str = get_stocks_info_json(fund_code, top_n=top_n)

    fund = session.get(FundInfoFlex, fund_code)
    if fund is None:
        fund = FundInfoFlex(fund_code=fund_code)
        session.add(fund)

    fund.stocks_info = json_str
    fund.update_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    session.commit()
    return json_str


if __name__ == "__main__":
    import sys

    fund_code = sys.argv[1] if len(sys.argv) > 1 else "001938"
    top_n = int(sys.argv[2]) if len(sys.argv) > 2 else 50

    json_str = get_stocks_info_json(fund_code, top_n=top_n)
    print(json_str)   # 只输出 JSON，方便管道给 C++