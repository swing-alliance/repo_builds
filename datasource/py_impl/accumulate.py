#这是accumulate.py
import os
# 【核心修复】同样在顶部配置内存分配，确保多线程和子函数调用安全
os.environ["PYTHONMALLOC"] = "malloc"
import threading
from diskio.fileio import *
from diskio.dbio import *
from netio.ak_netio_fund import *
from static.cal import *
import pandas as pd
from db_obj import FundInfo,init_sqlite_db
from typing import List
import json
from typing import List, Optional
import math
#能够在任意目录下拉起系统，适配老版本findoutfind，初试化系统可以执行先后顺序为pull_fund_to_dir,然后update_funds_info，最后flush_fund
#内存紧张可以先一部分pull_fund_to_dir,然后update_funds_info，最后flush_fund如此往复

#文件系统io+网络io(成熟系统使用，io沉重)
def update_fund_by_dir(target_dir: str, fetch_thread: int, write_thread: int, chunk_size: int = 300):
    """
    扫描干净目录，所有xxx.csv（文件名=6位基金代码）分批更新
    内部核心执行逻辑，直接同步跑，避免线程套线程池引起的 C++ 底层冲突，分批处理控制内存峰值
    :param target_dir: csv目录路径
    :param fetch_thread: 网络拉取并发线程数
    :param write_thread: 磁盘写入并发线程数
    :param chunk_size: 每批基金数量，控制内存峰值
    """
    code_list = []
    if not os.path.exists(target_dir):
        print(f"目录不存在: {target_dir}")
        return
    # 扫描目录，收集6位数字基金代码
    for fname in os.listdir(target_dir):
        if fname.lower().endswith(".csv"):
            code = os.path.splitext(fname)[0]
            if len(code) == 6 and code.isdigit():
                code_list.append(code)
    if not code_list:
        print("目录下无6位数字命名的csv文件，退出")
        return
    print(f"找到{len(code_list)}只基金，分批大小：{chunk_size}")
    total_cnt = len(code_list)
    # 循环切分code_list分批执行
    for batch_idx, offset in enumerate(range(0, total_cnt, chunk_size)):
        batch_codes = code_list[offset : offset + chunk_size]
        print(f"\n===== 第 {batch_idx+1} 批，[{offset} ~ {offset + len(batch_codes)-1}]，共 {len(batch_codes)} 个基金 =====")
        # 增加停止标记，适配新版 ak_netio_fund
        stop_flag = threading.Event()
        res = multi_instf_netio_fund(batch_codes, max_workers=fetch_thread, stop_flag=stop_flag)
        valid_packs = []
        for code, pack in zip(batch_codes, res):
            if pack is not None:
                print(f"✅基金{pack.code}，记录行数：{len(pack.df)}")
                valid_packs.append(pack)
            else:
                print(f"❌基金 {code} 拉取失败")
        print(f"\n本批成功拉取{len(valid_packs)}个基金，开始落地CSV，写线程={write_thread}")
        # 重点：mk_update_fund_batch内部不要再新建ThreadPoolExecutor！
        if valid_packs:
            mk_update_fund_batch(target_dir, valid_packs, max_workers=write_thread)
        else:
            print("ℹ️本批无有效数据，跳过写入")
        # 释放本批对象，降低内存占用
        del res
        del valid_packs
        del stop_flag

    print("\n✅全部批次拉取+文件写入全部完成")

#文件系统io+数据库FundInfo表io(大更新或者初始化使用，io轻量)
def flush_fund(target_dir: str, db_dir: str, days: int):
    """
    【调用即全量检查+同步清理】
    强制检查：1. 数据库垃圾/无效基金  2. 本地目录过期基金  3. 同前缀多份额的 A 份额清理
    同步执行：数据库主表+动态表删除、本地CSV文件删除，两边范围完全一致
    """
    # ========== 1. 强制执行各类检查（调用必跑，不会提前跳过） ==========
    # 1.1 检查数据库：垃圾债基 / 无效基金
    unwanteds: List[FundInfo] = get_unwanted_funds(db_dir)
    bond_codes = [item.fund_code for item in unwanteds]

    # 1.2 检查数据库：同前缀多份额需要清理的 A 份额（新增！）
    pair_a_unwanteds: List[FundInfo] = get_pair_share_A_to_remove(db_dir)
    pair_a_codes = [item.fund_code for item in pair_a_unwanteds]

    # 1.3 检查本地文件：过期退市基金（目录不存在也正常返回空，不崩溃）
    overdue_codes = []
    if os.path.isdir(target_dir):
        lists = load_fund_batch(target_dir)
        list_todel = get_overdue(lists, days)
        overdue_codes = [f.code for f in list_todel]

    # ========== 2. 合并全量待清理范围（加入 pair_a_codes） ==========
    all_delete_codes = list(set(bond_codes + pair_a_codes + overdue_codes))

    # ========== 3. 统一输出检查结果 ==========
    print(f"🔍 基金清理检查完成：")
    print(f"   - 数据库无效/垃圾债基：{len(bond_codes)} 只")
    print(f"   - 同前缀多份额需清理的A份额：{len(pair_a_codes)} 只")
    print(f"   - 本地文件过期退市：{len(overdue_codes)} 只")
    print(f"   - 合并去重待清理：{len(all_delete_codes)} 只")

    if not all_delete_codes:
        print("✅ 无需要清理的基金，数据库与本地文件均正常")
        print("\nℹ️ 清理检查执行完毕，无需处理")
        return

    print(f"   - 待清理代码列表：{all_delete_codes}")

    # ========== 4. 同步清理数据库 ==========
    delete_fund_info_db_batch(all_delete_codes, db_dir)
    print(f"✅ 已删除 fund_info 主表记录 {len(all_delete_codes)} 条")

    # ========== 5. 同步清理本地CSV文件 ==========
    deleted_file_cnt = del_dir_codes(target_dir, all_delete_codes)
    print(f"✅ 同步删除本地CSV文件 {deleted_file_cnt} 个")

    # ========== 6. 最终总结 ==========
    print("\n🎉 全部清理任务执行完毕，数据库与本地文件同步")

#数据库io以及文件io(大更新或者初始化使用)危险危险危险
def flush_no_info(target_dir: str):
    """
    【调用即全量检查+同步清理】
    强制检查：数据库中不存在记录的基金
    同步执行：本地CSV文件删除
    """
    # 1. 检查数据库中不存在的基金代码
    codes_in_dir = get_target_dir_codes(target_dir)
    codes_not_in_db = get_codes_not_in_db(codes_in_dir, db_relative_path="../app.db")
    if not codes_not_in_db:
        print("✅ 数据库中存在所有基金记录，无需清理")
        return
    print(f"🔍 数据库中不存在的基金代码：{codes_not_in_db}")
    # 2. 同步清理本地CSV文件
    deleted_file_cnt = del_dir_codes(target_dir, codes_not_in_db)
    print(f"✅ 已删除本地CSV文件 {deleted_file_cnt} 个")
    print("\n🎉 全部清理任务执行完毕，数据库与本地文件同步")


#文件系统io+网络io(大更新或者初始化使用，io沉重)
def pull_fund_to_dir(target_dir, startnum, endnum, threads: int, chunk_size: int = 300):
    """拉取基金数据到目录，增量；切割code_list，每一批拉完就调用一次mk_update_fund_batch
    ⚠️ mk_update_fund_batch自带多线程，每一批处理完再进入下一批
    """
    code_list = gen_fund_code_list(startnum, endnum)  # List[str]
    total_cnt = len(code_list)
    print(f"📋 总任务量：{total_cnt} 条基金，每 {chunk_size} 条为一批")
    total_success = 0
    total_fail = 0
    for batch_idx, offset in enumerate(range(0, total_cnt, chunk_size)):
        batch_codes = code_list[offset : offset + chunk_size]
        print(f"\n===== 第 {batch_idx+1} 批，[{offset} ~ {offset + len(batch_codes)-1}]，共 {len(batch_codes)} 个基金 =====")
        stop_flag = threading.Event()
        mem_inst = multi_instf_netio_fund(batch_codes, threads, stop_flag=stop_flag)
        valid_packs = []
        for code, pack in zip(batch_codes, mem_inst):
            if pack is not None:
                valid_packs.append(pack)
            else:
                print(f"❌基金 {code} 拉取失败，跳过写入")
        batch_success = len(valid_packs)
        batch_fail = len(batch_codes) - batch_success
        total_success += batch_success
        total_fail += batch_fail
        if valid_packs:
            print(f"✅本批有效 {batch_success} 条，调用 mk_update_fund_batch 写入")
            mk_update_fund_batch(target_dir, valid_packs, max_workers=3)
        else:
            print(f"ℹ️本批全部拉取失败，跳过写入")
        # 释放本批对象，减少内存占用
        del valid_packs
        del mem_inst
        del stop_flag
    print(f"\n====================全部拉取完成====================")
    print(f"✅成功：{total_success} | ❌失败：{total_fail}")

#数据库FundInfo表io+网络io+文件系统读取io(大更新或者初始化使用，io沉重)
def update_funds_info(
    target_dir: str, 
    db_dir: str, 
    max_workers: int, 
    batch_split_num: int = 3, 
    start_i: int = 0
) -> None:
    """
    存量更新target_dir下的基金，目的地db_dir,增量更新到数据库
    将全部基金代码切分为多批分批处理，避免一次性加载全部数据撑爆内存
    【改造后】支持UPSERT：DB存在则更新；DB不存在则新建入库
    :param target_dir: 目标目录
    :param db_dir: sqlite数据库相对路径
    :param max_workers: 网络拉取线程数
    :param batch_split_num: 分片数量
    :param start_i: 起始基金代码过滤阈值（基金代码转成整数后小于此值的将被过滤跳过）
    """
    try:
        codes_list: List[str] = get_target_dir_codes(target_dir)
        
        # ------------------ 新增：根据 start_i 向前筛选 ------------------
        if start_i > 0:
            original_cnt = len(codes_list)
            codes_list = [code for code in codes_list if code.isdigit() and int(code) >= start_i]
            print(f"🔍 依据 start_i={start_i} 筛选：原基金数 {original_cnt} 条，过滤后剩余 {len(codes_list)} 条")
        # -----------------------------------------------------------------
        total_cnt = len(codes_list)
        if total_cnt == 0:
            print("⚠️ 过滤后或target_dir未读取到任何基金代码，本次更新直接结束")
            return
        print(f"📋 共获取基金代码 {total_cnt} 条，拆分为 {batch_split_num} 批执行")
        chunk_size = math.ceil(total_cnt / batch_split_num)
        code_chunks = [codes_list[i:i+chunk_size] for i in range(0, total_cnt, chunk_size)]
        
        for batch_idx, code_chunk in enumerate(code_chunks, start=1):
            chunk_len = len(code_chunk)
            print(f"\n===== 开始第 {batch_idx}/{batch_split_num} 批，本批基金数量：{chunk_len} =====")
            try:
                # 1. 多线程拉取线上数据
                df_list: List[Optional[pd.DataFrame]] = fetch_fund_latest_info_batch_thread(code_chunk, max_workers)
                online_map = {}
                for df in df_list:
                    if df is None or df.empty:
                        continue
                    for _, row in df.iterrows():
                        code = row["基金代码"]
                        online_map[code] = {
                            "fund_name": row["基金简称"],
                            "fund_type": row["基金类型"],
                            "fund_info": row["基金信息"],
                            "update_time": pd.Timestamp.now().strftime("%Y-%m-%d %H:%M:%S")
                        }
                print(f"✅ 本批线上拉取有效记录：{len(online_map)}")
                # 2. 查询DB存量
                fundinfos: List[Optional[FundInfo]] = get_fund_info_batch_db(code_chunk, db_dir)
                db_exist_map = {}
                for fundinfo in fundinfos:
                    if fundinfo is not None:
                        db_exist_map[fundinfo.fund_code] = fundinfo
                update_candidate_list: List[FundInfo] = []
                # 遍历线上拿到的全部基金
                for code, online_data in online_map.items():
                    if code in db_exist_map:
                        # DB存在：更新原有对象字段
                        fundinfo = db_exist_map[code]
                        fundinfo.fund_name = online_data["fund_name"]
                        fundinfo.fund_type = online_data["fund_type"]
                        fundinfo.fund_info = online_data["fund_info"]
                        fundinfo.update_time = online_data["update_time"]
                        update_candidate_list.append(fundinfo)
                    else:
                        # DB不存在：新建FundInfo对象
                        new_fund = FundInfo(
                            fund_code=code,
                            fund_name=online_data["fund_name"],
                            fund_type=online_data["fund_type"],
                            fund_info=online_data["fund_info"],
                            update_time=online_data["update_time"],
                        )
                        update_candidate_list.append(new_fund) 
                print(f"📊 本批待入库：{len(update_candidate_list)} 条")
                if update_candidate_list:
                    batch_success = upsert_fund_info_db_batch(update_candidate_list, db_dir)
                    if not batch_success:
                        raise Exception("批量upsert返回false")
                    print(f"✅ 第 {batch_idx} 批执行完成")
                else:
                    print(f"📌【提示】本批无待更新记录，跳过入库调用")
            except Exception as batch_e:
                print(f"❌ 第 {batch_idx} 批处理异常：{batch_e}，继续执行下一批")
                continue 
        print("\n🎉 所有批次执行完毕！")
    except Exception as e:
        print(f"❌ 读取基金代码列表发生异常，错误信息：{e}")
        return

#数据库FundInfoFlex表io+网络io(成熟系统使用,io沉重)
def update_fund_stock_holding(db_dir: str,max_workers: int,batch_split_num: int,codes: List[str],) -> None:
    """
    对 codes 列表中的基金进行股票持仓更新，并写进数据库。

    参数：
        db_dir:          数据库路径，如 "data/fund.db"
        max_workers:     拉取线程数
        batch_split_num: 每批处理的基金数量
        codes:           待更新的基金代码列表

    流程：
        1. codes 按 batch_split_num 分批
        2. 每批调用 fetch_fund_latest_holdings_json_batch_thread 并发拉取
        3. 每批结果调用 upsert_fund_info_flex_stocks_db 写入
    """
    if not codes:
        print("⚠️ codes 为空，无需更新")
        return

    total = len(codes)
    batch_split_num = max(1, batch_split_num)
    success_total = 0

    print(f"🚀 开始更新基金持仓：共 {total} 只，"
          f"批大小 {batch_split_num}，线程数 {max_workers}")

    for start in range(0, total, batch_split_num):
        batch = codes[start : start + batch_split_num]
        batch_no = start // batch_split_num + 1
        batch_total = (total + batch_split_num - 1) // batch_split_num

        print(f"\n📦 第 {batch_no}/{batch_total} 批，"
              f"{len(batch)} 只：{batch[:5]}{'...' if len(batch) > 5 else ''}")

        # 1. 并发拉取
        try:
            result = fetch_fund_latest_holdings_json_batch_thread(
                code_list=batch,
                max_workers=max_workers,
            )
        except Exception as e:
            print(f"❌ 第 {batch_no} 批拉取异常：{e}")
            continue

        if not result:
            print(f"⚠️ 第 {batch_no} 批无有效数据，跳过写入")
            continue

        # 2. 写库
        ok = upsert_fund_info_flex_stocks_db(
            db_relative_path=db_dir,
            info_dict=result,
        )
        if ok:
            success_total += len(result)
            print(f"✅ 第 {batch_no} 批入库成功：{len(result)} 只")
        else:
            print(f"❌ 第 {batch_no} 批入库失败")

    print(f"\n🏁 全部完成：成功更新 {success_total}/{total} 只")

#文件系统io+数据库io(大更新或者初始化使用，io轻量)
def reload_db(db_dir:str):
    """重新创建数据库，如果有数据，清空数据"""
    init_sqlite_db(db_dir)


if __name__ == "__main__":
    #pull_fund_to_dir(target_dir="../funds_row/", startnum=150000, endnum=160000, threads=5,chunk_size=100)#150000-160000在10月3完成
    #update_funds_info("../funds_row/", "../app.db", 8, 10,30000)
    # update_fund_by_dir("../funds_row/",10,3,300)
    # reload_db("../app.db")
    #flush_no_info("../funds_row/")  #数据库中不存在记录的直接删除,相对风险的操作
    #flush_fund("../funds_row/", "../app.db", 30)  #大埔是的销毁,相对安全
    
    update_fund_stock_holding(db_dir="../app.db",max_workers=4,batch_split_num=50,codes=["000001", "000002", "000003"])
    # update_fund_stock_holding(db_dir="../app.db",max_workers=4,batch_split_num=50,codes=get_target_dir_codes("../funds_row"))
    pass