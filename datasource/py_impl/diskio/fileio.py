import os
import pandas as pd
from typing import List
from concurrent.futures import ThreadPoolExecutor
from py_obj import FundInst

def mk_update_fund(funds_dir_path, pack: FundInst):
    """
    保存单只基金净值DataFrame到csv，以基金code命名，路径：funds_dir_path/{code}.csv
    :param funds_dir_path: 目标文件夹路径
    :param pack: 单只基金FundInst数据包（内含code与完整df）
    """
    try:
        # 1. 自动转换为绝对路径，防止路径飘移
        abs_dir_path = os.path.abspath(funds_dir_path)
        os.makedirs(abs_dir_path, exist_ok=True)
        
        fund_code = pack.code
        file_path = os.path.join(abs_dir_path, f"{fund_code}.csv")

        # 执行写入
        pack.df.to_csv(file_path, index=False, encoding="utf-8-sig")
        print(f"✅ 保存csv成功：{file_path}，共{len(pack.df)}行")
        
    except Exception as e:
        print(f"❌ [写入崩溃异常] 基金 {pack.code} 写入失败，原因: {e}")

def mk_update_fund_batch(funds_dir_path, pack_list: List[FundInst], max_workers: int = 4):
    """
    多线程批量写入多只基金csv
    :param funds_dir_path: 目标文件夹路径
    :param pack_list: FundInst对象列表
    :param max_workers: 写文件线程数量，磁盘IO建议2~4
    """
    valid_packs = [p for p in pack_list if p is not None]
    if not valid_packs:
        print("⚠️ 无有效基金数据包可供写入")
        return
    
    with ThreadPoolExecutor(max_workers=max_workers) as executor:
        executor.map(lambda p: mk_update_fund(funds_dir_path, p), valid_packs)

def load_fund_batch(csv_dir: str) -> List[FundInst]:
    """
    磁盘到内存：批量读取目录下所有6位数字命名csv，打包成List[FundInst]
    :param csv_dir: 基金csv目录
    :return: FundInst对象列表
    """
    pack_list: List[FundInst] = []
    abs_dir_path = os.path.abspath(csv_dir)

    if not os.path.exists(abs_dir_path):
        print(f"⚠️ 读取目录不存在：{abs_dir_path}")
        return pack_list
        
    for fname in os.listdir(abs_dir_path):
        if fname.lower().endswith(".csv"):
            code = os.path.splitext(fname)[0]
            if len(code) == 6 and code.isdigit():
                file_path = os.path.join(abs_dir_path, fname)
                try:
                    df = pd.read_csv(file_path, encoding="utf-8-sig")
                    pack = FundInst(code, df)
                    pack_list.append(pack)
                except Exception as e:
                    print(f"❌ 读取文件 {file_path} 失败: {e}")
                    
    return pack_list

def delete_fund_by_list(target_dir: str, fund_list: List[FundInst]):
    """
    单线程：对照List[FundInst]，在目标目录删除对应code的csv文件
    """
    abs_dir_path = os.path.abspath(target_dir)
    if not os.path.exists(abs_dir_path):
        print(f"⚠️ 目录不存在：{abs_dir_path}")
        return
        
    exist_files = set(os.listdir(abs_dir_path))
    for pack in fund_list:
        code = pack.code
        filename = f"{code}.csv"
        if filename in exist_files:
            csv_path = os.path.join(abs_dir_path, filename)
            try:
                os.remove(csv_path)
                print(f"🗑️ 已删除：{csv_path}")
            except Exception as e:
                print(f"❌ 删除文件 {csv_path} 失败: {e}")