import sys
import os
import time
from accumulate import *


def _input_codes() -> list:
    """手动输入基金代码，支持逗号/空格分隔"""
    raw = input("请输入基金代码（逗号或空格分隔，如 000001,000002）: ").strip()
    if not raw:
        return []
    # 逗号或空格都能拆
    parts = raw.replace(",", " ").split()
    # 去重且保持顺序
    seen = set()
    codes = []
    for c in parts:
        c = c.strip()
        if c and c not in seen:
            seen.add(c)
            codes.append(c)
    return codes


if __name__ == "__main__":
    print("=======================更新管理工具,仅仅支持对国内相关基金的更新,清洗 v1.0.2 =======================\n")

    # 纯粹接收你输入的路径，支持任意绝对路径（如 D:\data\funds）或相对路径
    target_dir = input("请输入目标基金csv目录（支持绝对路径或相对路径）: ").strip()
    db_path = input("请输入目标db的相对路径: ").strip()

    if not target_dir:
        target_dir = "funds_row"  # 仅仅在完全空回车时给个默认名

    print(f"🔍 [调试] 当前使用csv目录: {target_dir}")
    print(f"🔍 [调试] 当前数据库路径: {db_path}\n")

    while True:
        print("\n---------------- 菜单 ----------------")
        print("1-拉取添加（按基金号段批量下载csv）")
        print("2-存量更新（按csv目录增量更新FundInfo）")
        print("3-清洗（过期+过滤债/定期基金）")
        print("4-按代码更新股票持仓（手动输入code）")
        print("5-重新加载db（从csv重建数据库）")
        print("6-退出")
        print("--------------------------------------")

        op_code = input("请输入操作码 1-6: ").strip()

        # ---------- 1 拉取添加 ----------
        if op_code == "1":
            print("=== 拉取添加 ===")
            try:
                startnum = int(input("请输入起始基金号码:"))
                endnum = int(input("请输入结束基金号码:"))
                threads = int(input("请输入并发线程数:"))
                pull_fund_to_dir(
                    target_dir=target_dir,
                    startnum=startnum,
                    endnum=endnum,
                    threads=threads,
                )
            except ValueError:
                print("❌ 输入不是数字，本次操作取消")

        # ---------- 2 存量更新 ----------
        elif op_code == "2":
            print("=== 存量更新 ===")
            try:
                net_thread = int(input("请输入网络并发线程数:"))
                update_fund_by_dir(target_dir, net_thread, 3)
            except ValueError:
                print("❌ 输入不是数字，本次操作取消")

        # ---------- 3 清洗 ----------
        elif op_code == "3":
            print("=== 清洗【过期基金 + 债基/定期基金】风险操作，谨慎预检，输入三次YES确认 ===")
            check1 = input("请输入第1次 YES 确认:")
            check2 = input("请输入第2次 YES 确认:")
            check3 = input("请输入第3次 YES 确认:")
            if check1 != "YES" or check2 != "YES" or check3 != "YES":
                print("校验失败，取消清洗")
                continue
            try:
                days = int(input("请输入过期天数以清洗:"))
                flush_fund(target_dir, db_path, days)
                print("✅ 清洗任务全部执行完毕！")
            except ValueError:
                print("❌ 天数必须是整数，操作取消")

        # ---------- 4 按代码更新股票持仓 ----------
        elif op_code == "4":
            print("=== 按代码更新股票持仓 ===")
            codes = _input_codes()
            if not codes:
                print("❌ 未输入有效基金代码，取消")
                continue
            try:
                max_workers = int(input("请输入网络并发线程数(建议1-2): ") or "2")
                batch_split_num = int(input("请输入每批基金数(默认50): ") or "50")
            except ValueError:
                print("❌ 输入不是数字，取消")
                continue
            print(f"🔍 将更新 {len(codes)} 只基金持仓: {codes[:5]}{'...' if len(codes) > 5 else ''}")
            update_fund_stock_holding(
                db_dir=db_path,
                max_workers=max_workers,
                batch_split_num=batch_split_num,
                codes=codes,
            )
            print("✅ 股票持仓更新任务执行完毕！")

        # ---------- 5 重新加载 db ----------
        elif op_code == "5":
            print("=== 重新加载db（从csv重建数据库）===")
            try:
                reload_db(db_path)
                print("✅ 数据库重建完成！")
            except Exception as e:
                print(f"❌ 重建失败: {e}")

        # ---------- 6 退出 ----------
        elif op_code == "6":
            print("退出")
            break

        else:
            print("❌ 无效的操作码，请重新输入。")

    print("\n=======================再见 v1.0.2 =======================\n")
    time.sleep(3)