# py_obj.py
from dataclasses import dataclass
import pandas as pd

@dataclass
class FundInst:
    """基金单条净值实例"""
    code: str           # 基金代码
    df:pd.DataFrame