import sqlalchemy
from sqlalchemy import create_engine, or_
from sqlalchemy.orm import sessionmaker
from sqlalchemy.exc import IntegrityError
from sqlalchemy.dialects.sqlite import insert as sqlite_insert 
from typing import List, Optional,Dict
import datetime
from collections import defaultdict

# 从 db_obj 导入所有ORM模型
# 注意：如果你的模型文件里分组类名还是 group_info，请改成 GroupInfo 保持规范
from db_obj import FundInfo, FundInfoFlex, GroupInfo


# ===================== FundInfo（静态冷表）CRUD =====================
def get_fund_info_batch_db(codes: List[str], db_relative_path: str) -> List[Optional[FundInfo]]:
    """
    批量查询fund_info表，一次性IN查询，性能优于循环单条查询
    :param codes: 6位基金代码字符串列表
    :param db_relative_path: sqlite数据库相对路径，支持 ../
    :return: 结果列表顺序与输入codes保持一致；存在返回FundInfo对象，不存在则为None
    """
    for c in codes:
        if not (len(c) == 6 and c.isdigit()):
            raise ValueError(f"无效基金代码: {c}，必须为6位数字字符串")
    conn_url = f"sqlite:///{db_relative_path}"
    engine = create_engine(conn_url)
    SessionLocal = sessionmaker(bind=engine)
    with SessionLocal() as session:
        obj_list = session.query(FundInfo).filter(FundInfo.fund_code.in_(codes)).all()
        obj_map = {obj.fund_code: obj for obj in obj_list}
    res = []
    for code in codes:
        res.append(obj_map.get(code, None))
    return res


def get_fund_info_db(code: str, db_relative_path: str) -> Optional[FundInfo]:
    """
    根据6位基金代码从fund_info表查询，返回FundInfo ORM对象，无数据返回None
    :param code: 6位字符串基金代码，例 "000001"
    :param db_relative_path: sqlite数据库相对路径，支持../ 如 "../app.db"
    :return: FundInfo对象 / None
    """
    if not (len(code) == 6 and code.isdigit()):
        raise ValueError("基金代码必须为6位数字字符串，示例：000001")
    conn_url = f"sqlite:///{db_relative_path}"
    engine = create_engine(conn_url)
    SessionLocal = sessionmaker(bind=engine)
    with SessionLocal() as session:
        fund_obj = session.query(FundInfo).filter(FundInfo.fund_code == code).first()
        return fund_obj


def add_fund_info_db(fund_obj: FundInfo, db_relative_path: str) -> bool:
    """新增基金记录，主键冲突会返回False"""
    conn_url = f"sqlite:///{db_relative_path}"
    engine = create_engine(conn_url)
    SessionLocal = sessionmaker(bind=engine)
    try:
        with SessionLocal() as session:
            session.add(fund_obj)
            session.commit()
        return True
    except IntegrityError:
        return False


def update_fund_info_db(fund_obj: FundInfo, db_relative_path: str) -> bool:
    """更新已有基金记录，依靠主键fund_code"""
    conn_url = f"sqlite:///{db_relative_path}"
    engine = create_engine(conn_url)
    SessionLocal = sessionmaker(bind=engine)
    with SessionLocal() as session:
        exist = session.get(FundInfo, fund_obj.fund_code)
        if not exist:
            return False
        # 仅更新 FundInfo 自身静态字段
        exist.fund_name = fund_obj.fund_name
        exist.fund_type = fund_obj.fund_type
        exist.fund_info = fund_obj.fund_info
        exist.fund_valid = fund_obj.fund_valid
        exist.update_time = fund_obj.update_time
        session.commit()
        return True


def upsert_fund_info_db(fund_obj: FundInfo, db_relative_path: str) -> bool:
    """存在就更新，不存在则新增（最常用）"""
    conn_url = f"sqlite:///{db_relative_path}"
    engine = create_engine(conn_url)
    SessionLocal = sessionmaker(bind=engine)
    with SessionLocal() as session:
        exist = session.get(FundInfo, fund_obj.fund_code)
        if exist:
            exist.fund_name = fund_obj.fund_name
            exist.fund_type = fund_obj.fund_type
            exist.fund_info = fund_obj.fund_info
            exist.fund_valid = fund_obj.fund_valid
            exist.update_time = fund_obj.update_time
        else:
            session.add(fund_obj)
        session.commit()
        return True


def upsert_fund_info_db_batch(fund_obj_list: List[FundInfo], db_relative_path: str) -> bool:
    """
    批量upsert FundInfo: 存在就更新，不存在则新增
    :param fund_obj_list: FundInfo对象列表
    :param db_relative_path: sqlite数据库相对路径
    :return: True 整体提交成功；False 出现异常回滚
    """
    if not fund_obj_list:
        print("⚠️ 传入空列表，无需入库")
        return True
    conn_url = f"sqlite:///{db_relative_path}"
    engine = create_engine(conn_url)
    SessionLocal = sessionmaker(bind=engine)
    try:
        with SessionLocal() as session:
            code_list = [obj.fund_code for obj in fund_obj_list]
            exist_records = session.query(FundInfo).filter(FundInfo.fund_code.in_(code_list)).all()
            exist_map = {rec.fund_code: rec for rec in exist_records}
            for fund_obj in fund_obj_list:
                rec = exist_map.get(fund_obj.fund_code)
                if rec:
                    rec.fund_name = fund_obj.fund_name
                    rec.fund_type = fund_obj.fund_type
                    rec.fund_info = fund_obj.fund_info
                    rec.fund_valid = fund_obj.fund_valid
                    rec.update_time = fund_obj.update_time
                else:
                    session.add(fund_obj)
            session.commit()
        return True
    except Exception as e:
        print(f"❌批量upsert FundInfo失败，发生异常: {e}")
        return False


def delete_fund_info_db(code: str, db_relative_path: str) -> bool:
    """按基金代码删除记录"""
    if not (len(code) == 6 and code.isdigit()):
        raise ValueError("基金代码必须为6位数字字符串，示例：000001")
    conn_url = f"sqlite:///{db_relative_path}"
    engine = create_engine(conn_url)
    SessionLocal = sessionmaker(bind=engine)
    with SessionLocal() as session:
        fund = session.get(FundInfo, code)
        if not fund:
            return False
        session.delete(fund)
        session.commit()
        return True


def delete_fund_info_db_batch(code_list: List[str], db_relative_path: str) -> int:
    """
    批量按基金代码删除FundInfo记录
    :param code_list: 6位数字基金代码列表
    :param db_relative_path: sqlite数据库相对路径
    :return: 成功删除的记录条数
    """
    for code in code_list:
        if not (len(code) == 6 and code.isdigit()):
            raise ValueError(f"基金代码必须为6位数字字符串，非法代码：{code}")
    conn_url = f"sqlite:///{db_relative_path}"
    engine = create_engine(conn_url)
    SessionLocal = sessionmaker(bind=engine)
    with SessionLocal() as session:
        funds = session.query(FundInfo).filter(FundInfo.fund_code.in_(code_list)).all()
        if not funds:
            return 0
        for f in funds:
            session.delete(f)
        session.commit()
        return len(funds)


def get_unwanted_funds(db_relative_path: str) -> List[FundInfo]:
    """
    查询不需要保留的基金记录：
    1. fund_type 为空
    2. fund_type 包含 "债"（债券类）
    3. fund_name 包含 "定期"
    4. fund_name 包含 "债"
    5.包含"固收"
    【不再直接过滤所有A结尾】
    返回满足任一条件的FundInfo对象列表
    """
    conn_url = f"sqlite:///{db_relative_path}"
    engine = create_engine(conn_url)
    SessionLocal = sessionmaker(bind=engine)
    with SessionLocal() as session:
        query = session.query(FundInfo).filter(
            or_(
                FundInfo.fund_type.is_(None),
                FundInfo.fund_type == "",
                FundInfo.fund_type.like("%固收%"),
                FundInfo.fund_type.like("%债%"),
                FundInfo.fund_name.like("%定期%"),
                FundInfo.fund_name.like("%债%"),
                FundInfo.fund_name.like("%持有%"),
            )
        )
        unwanted_list: List[FundInfo] = query.all()
        return unwanted_list


def get_pair_share_A_to_remove(db_relative_path: str) -> List[FundInfo]:
    """
    bug版：根据基金名称（去除尾部的 A/C 等后缀）来识别同名多份额的 A 份额
    """
    conn_url = f"sqlite:///{db_relative_path}"
    engine = create_engine(conn_url)
    SessionLocal = sessionmaker(bind=engine)
    
    with SessionLocal() as session:
        all_valid = session.query(FundInfo).where(FundInfo.fund_valid == 1).all()
        
        group = defaultdict(set)
        code_map = {}
        
        for item in all_valid:
            code = item.fund_code
            name = item.fund_name  # 假设字段名为 fund_name，根据实际调整
            if not code or not name:
                continue
                
            code_map[code] = item
            
            # 根据基金名称来清洗和归类
            # 例如 "华夏大盘精选混合A" -> 基础名称是 "华夏大盘精选混合", 后缀是 "A"
            clean_name = name.strip()
            
            # 判断名称是否以 A、B、C 结尾（支持全角或半角括号，或者直接结尾）
            # 这里以常见的情况为例：如果名字以 A/C 结尾
            if clean_name.endswith(("A", "C", "B", "（A）", "（C）", "(A)", "(C)")):
                # 提取纯净的基金名称作为组名
                if clean_name.endswith(("A", "C", "B")):
                    base_name = clean_name[:-1].rstrip()
                    suffix = clean_name[-1]
                else:
                    base_name = clean_name[:-3].rstrip()
                    suffix = clean_name[-2] # 获取括号内的字母
                
                group[base_name].add(suffix)
                # 顺便把对应关系存起来，方便后面找 A 份额的代码
                if suffix == "A":
                    group[base_name].add(("A_CODE", code))
            else:
                group[clean_name].add("OTHER")

        remove_codes = []
        for base_name, suffix_set in group.items():
            # 如果该基金同时存在 A 以及 (C 或 B)
            has_a = "A" in suffix_set
            has_bc = "C" in suffix_set or "B" in suffix_set
            
            if has_a and has_bc:
                # 把该组里记录的 A 份额代码找出来
                for item in suffix_set:
                    if isinstance(item, tuple) and item[0] == "A_CODE":
                        remove_codes.append(item[1])

        res = [code_map[c] for c in remove_codes if c in code_map]
        return res


def get_codes_not_in_db(all_codes: List[str], db_relative_path: str) -> List[str]:
    """
    查询一批基金代码中哪些不在数据库中
    :param all_codes: 6位数字基金代码列表
    :param db_relative_path: sqlite数据库相对路径
    :return: 不存在的基金代码列表
    """
    for code in all_codes:
        if not (len(code) == 6 and code.isdigit()):
            raise ValueError(f"基金代码必须为6位数字字符串，非法代码：{code}")
    conn_url = f"sqlite:///{db_relative_path}"
    engine = create_engine(conn_url)
    SessionLocal = sessionmaker(bind=engine)
    with SessionLocal() as session:
        existing_codes = session.query(FundInfo.fund_code).filter(FundInfo.fund_code.in_(all_codes)).all()
        existing_set = {code for (code,) in existing_codes}
        not_in_db = [code for code in all_codes if code not in existing_set]
        return not_in_db
   


# ===================== FundInfoFlex（动态热表）CRUD =====================
def upsert_fund_info_flex_stocks_db(
    db_relative_path: str,
    info_dict: Dict[str, str],
) -> bool:
    """
    批量 upsert 基金最新持仓。
    - fund_code 不存在 → 新增（stocks_info / update_time 写入，其余走默认）
    - fund_code 已存在 → 只更新 stocks_info 和 update_time
    """
    if not info_dict:
        return True

    conn_url = f"sqlite:///{db_relative_path}"
    engine = create_engine(conn_url)
    SessionLocal = sessionmaker(bind=engine)

    now_str = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    rows = [
        {
            "fund_code": code,
            "stocks_info": json_str,
            "update_time": now_str,
        }
        for code, json_str in info_dict.items()
    ]

    try:
        with SessionLocal() as session:
            for i in range(0, len(rows), 500):
                chunk = rows[i : i + 500]
                stmt = sqlite_insert(FundInfoFlex).values(chunk)
                stmt = stmt.on_conflict_do_update(
                    index_elements=[FundInfoFlex.fund_code],
                    set_={
                        "stocks_info": stmt.excluded.stocks_info,
                        "update_time": stmt.excluded.update_time,
                    },
                )
                session.execute(stmt)
            session.commit()
        return True
    except Exception as e:
        print(f"❌ upsert_fund_info_flex_stocks_db 失败: {e}")
        return False

# ===================== GroupInfo（分组表）CRUD =====================
def get_all_groups(db_relative_path: str) -> List[GroupInfo]:
    """查询所有分组"""
    conn_url = f"sqlite:///{db_relative_path}"
    engine = create_engine(conn_url)
    SessionLocal = sessionmaker(bind=engine)
    with SessionLocal() as session:
        return session.query(GroupInfo).all()


def get_group_db(group_name: str, db_relative_path: str) -> Optional[GroupInfo]:
    """按分组名查询分组详情"""
    conn_url = f"sqlite:///{db_relative_path}"
    engine = create_engine(conn_url)
    SessionLocal = sessionmaker(bind=engine)
    with SessionLocal() as session:
        return session.get(GroupInfo, group_name)


def upsert_group_db(group_obj: GroupInfo, db_relative_path: str) -> bool:
    """分组信息upsert"""
    conn_url = f"sqlite:///{db_relative_path}"
    engine = create_engine(conn_url)
    SessionLocal = sessionmaker(bind=engine)
    with SessionLocal() as session:
        exist = session.get(GroupInfo, group_obj.group_name)
        if exist:
            exist.group_members = group_obj.group_members
        else:
            session.add(group_obj)
        session.commit()
        return True


# 测试入口
if __name__ == "__main__":
    pass