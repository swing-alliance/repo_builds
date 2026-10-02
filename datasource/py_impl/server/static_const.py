from pathlib import Path


def get_funds_row_path() -> str:
  """自动向上查找 datasource 目录，并拼接返回 funds_row 文件夹的绝对路径"""
  current_path = Path(__file__).resolve()

  # 向上遍历所有父级目录，寻找名为 "datasource" 的文件夹
  for parent in current_path.parents:
    if parent.name == "datasource":
      target_dir = parent / "funds_row"
      return str(target_dir)

  # 兜底方案：如果目录结构变动导致没匹配到，默认向上回退两级拼接
  fallback_dir = current_path.parent.parent / "funds_row"
  return str(fallback_dir.resolve())


# 测试代码（直接运行该文件时可打印查看）
if __name__ == "__main__":
  print("funds_row 绝对路径:", get_funds_row_path())