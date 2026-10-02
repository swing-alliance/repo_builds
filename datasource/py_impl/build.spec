import os
import pandas as pd
from PyInstaller.utils.hooks import collect_data_files, collect_dynamic_libs

# ================== 路径配置 ==================
py_mini_path = r"A:\fund_qt\io\py_impl\venv\Lib\site-packages\py_mini_racer"
calendar_path = r"A:\fund_qt\io\py_impl\venv\Lib\site-packages\akshare\file_fold"

# 自动收集 py_mini_racer 的所有数据文件和动态库
py_mini_datas = collect_data_files('py_mini_racer')
py_mini_binaries = collect_dynamic_libs('py_mini_racer')

a = Analysis(
    ['main.py'],
    pathex=[],
    binaries=[
        # 1. 自动收集的所有动态库
        *py_mini_binaries,
        # 2. 关键二进制文件
        (os.path.join(py_mini_path, 'mini_racer.dll'), 'py_mini_racer'),
        (os.path.join(py_mini_path, 'mini_racer.dll'), '.'),
        (os.path.join(py_mini_path, 'icudtl.dat'), 'py_mini_racer'),
        (os.path.join(py_mini_path, 'icudtl.dat'), '.'),  # V8 引擎的字典文件
    ],
    datas=[
        *py_mini_datas,
        # akshare 的 calendar.json
        (os.path.join(calendar_path, 'calendar.json'), 'akshare\\file_fold'),
    ],
    hiddenimports=[
        'py_mini_racer',
    ],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
    optimize=0,
)

pyz = PYZ(a.pure)

# ⚠️ 注意这里：改成 exclude_binaries=True，适配文件夹模式 (onedir)
exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='manager',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=False,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=True,              # 保持控制台以便观察日志
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)

# 🚀 增加 COLLECT 块：以文件夹形式收集所有二进制和数据，让 V8 引擎平铺加载，告别内存崩溃！
coll = COLLECT(
    exe,
    a.binaries,
    a.datas,
    strip=False,
    upx=False,
    upx_exclude=[],
    name='manager',
)