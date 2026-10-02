from sqlalchemy import create_engine, Text, Integer
from sqlalchemy.orm import DeclarativeBase, Mapped, mapped_column
from pathlib import Path


class Base(DeclarativeBase):
    pass


class StockInfo(Base):
    __tablename__ = "stock_info"
    stock_code: Mapped[str] = mapped_column(Text, primary_key=True)
    stock_name: Mapped[str | None] = mapped_column(Text)
    stock_info: Mapped[str | None] = mapped_column(Text)
    stock_valid: Mapped[int | None] = mapped_column(Integer)
    update_time: Mapped[str | None] = mapped_column(Text)


class FundInfo(Base):#存储基金长久的信息，短时间不需要更新,冷表
    __tablename__ = "fund_info"
    fund_code: Mapped[str] = mapped_column(Text, primary_key=True)
    fund_name: Mapped[str | None] = mapped_column(Text, default=None)
    fund_type: Mapped[str | None] = mapped_column(Text, default=None)  # 基金类型，混合|股票等
    fund_info: Mapped[str | None] = mapped_column(Text, default=None)
    fund_valid: Mapped[int | None] = mapped_column(Integer, default=1)  # 0无效，1有效
    update_time: Mapped[str | None] = mapped_column(Text, default=None)



class FundInfoFlex(Base):#存储基金会经常变动的信息，短时间需要更新如基金是否被标记，基金属于哪些分组,热表
    __tablename__ = "fund_info_flex"
    fund_code: Mapped[str] = mapped_column(Text, primary_key=True)
    # 股票信息 JSON 数组字符串，只保留最新一期
    # 结构：[{"code","name","ratio","shares","value","quarter"}, ...]
    stocks_info: Mapped[str | None] = mapped_column(
        Text, default=None, nullable=True
    )
    mark_flag: Mapped[int | None] = mapped_column(Integer, default=0)  #0未标记，1标记
    group_name: Mapped[str | None] = mapped_column(Text, default=None)# 多分组 |分隔，示例"消费|指数|波段"；无分组存NULL
    update_time: Mapped[str | None] = mapped_column(Text, default=None)

class GroupInfo(Base):
    __tablename__ = "group_info"
    group_name: Mapped[str] = mapped_column(Text, primary_key=True)
    group_members: Mapped[str | None] = mapped_column(Text, default=None)


def group_list_to_str(groups: list[str]) -> str | None:
    """分组列表 -> |分隔字符串，空列表返回None"""
    if not groups:
        return None
    unique_groups = list(dict.fromkeys(groups))
    return "|".join(unique_groups)


def group_str_to_list(group_str: str | None) -> list[str]:
    """|分隔字符串 -> 分组列表，null返回空列表，剔除空白分组"""
    if group_str is None:
        return []
    return [g.strip() for g in group_str.split("|") if g.strip()]


def init_sqlite_db(db_relative_path: str, drop_if_exists: bool = True):
    """
    初始化sqlite数据库，支持 ../ 向上级目录
    :param db_relative_path: 相对路径，支持 ../ 向上一级，如 "../app.db"
    :param drop_if_exists: True：如果db文件存在，先删除再重建；False：存在则复用，只建不存在的表
    :return: engine对象
    """
    db_path = Path(db_relative_path)
    # resolve 自动解析 ../ ，获取真实绝对路径
    abs_db_path = db_path.resolve()
    if drop_if_exists and abs_db_path.exists():
        abs_db_path.unlink()
        print(f"🗑️ 已删除旧数据库文件：{abs_db_path}")
    abs_db_path.parent.mkdir(parents=True, exist_ok=True)
    conn_str = f"sqlite:///{abs_db_path}"
    engine = create_engine(conn_str)
    Base.metadata.create_all(engine)
    print(f"✅ 数据库初始化完成，绝对路径: {abs_db_path}")
    return engine


if __name__ == "__main__":
    # ========== 往上一级目录创建app.db ==========
    engine = init_sqlite_db("../app.db")
    # 上两级："../../app.db"
