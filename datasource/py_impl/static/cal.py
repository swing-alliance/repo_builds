#cal.py  存放静态方法，如对dataframe的操作
import pandas as pd
from typing import List
from py_obj import FundInst
import os
from datetime import timedelta
# FundInst中的df格式为   净值日期,累计净值,daily_return


def get_overdue(fis: List[FundInst], overdays: int) -> List[FundInst]:
    """如果dataframe中的最新日期逾期了overday了，返回要丢的实例列表"""
    discard_list: List[FundInst] = []
    #倒序遍历原地删除，收集要丢弃的实例
    for i in reversed(range(len(fis))):
        inst = fis[i]
        latest_dt = pd.to_datetime(inst.df["净值日期"]).max()
        time_gap = pd.Timestamp.now() - latest_dt
        if time_gap > timedelta(days=overdays):
            discard_list.append(inst)
            del fis[i]
    return discard_list

def gen_fund_code_list(start: int, end: int) -> List[str]:
    """
    输入起止整数，限制范围1~999999，生成6位补零字符串基金代码列表
    :param start: 起始数字
    :param end: 结束数字（包含）
    :return: List["000001","001002"]
    """
    # 校验区间
    if not (1 <= start <= 999999):
        raise ValueError("start必须在1 ~ 999999之间")
    if not (1 <= end <= 999999):
        raise ValueError("end必须在1 ~ 999999之间")
    if start > end:
        raise ValueError("start不能大于end")
    code_list: List[str] = [f"{num:06d}" for num in range(start, end + 1)]
    return code_list


def get_target_dir_codes(target_dir: str) -> List[str]:
    """获取csv目录下的基金代码列表"""
    code_list: List[str] = []
    for fname in os.listdir(target_dir):
        if fname.lower().endswith(".csv"):
            code = os.path.splitext(fname)[0]
            if len(code) == 6 and code.isdigit():
                code_list.append(code)
    return code_list

def del_dir_codes(target_dir: str, codes: List[str]) -> int:
    """
    批量删除目录下指定基金代码对应的CSV文件
    :param target_dir: csv目录路径
    :param codes: 待删除的基金代码列表
    :return: 实际成功删除的文件数量
    """
    if not os.path.isdir(target_dir):
        return 0
    # 转集合实现O(1)查找，批量场景性能提升明显
    code_set = set(codes)
    deleted_cnt = 0

    for fname in os.listdir(target_dir):
        if not fname.lower().endswith(".csv"):
            continue
        code = os.path.splitext(fname)[0]
        if code in code_set:
            full_path = os.path.join(target_dir, fname)
            try:
                os.remove(full_path)
                deleted_cnt += 1
            except OSError as e:
                print(f"⚠️ 删除文件失败 {fname}: {e}")
    return deleted_cnt
