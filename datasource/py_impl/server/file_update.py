import os
import sys
from pathlib import Path
from contextlib import asynccontextmanager
from fastapi import FastAPI, HTTPException, Response
from pydantic import BaseModel
import aiofiles  # 确保你安装了 pip install aiofiles，实现真正的异步文件读写
import uvicorn
from apscheduler.schedulers.background import BackgroundScheduler
from apscheduler.triggers.cron import CronTrigger

# 动态加入系统路径
current_dir = os.path.dirname(os.path.abspath(__file__))
py_impl_dir = os.path.abspath(os.path.join(current_dir, ".."))
if py_impl_dir not in sys.path:
    sys.path.insert(0, py_impl_dir)

from accumulate import *
from static_const import get_funds_row_path

def my_daily_update_task():
    update_fund_by_dir(get_funds_row_path(), 2, 2, 80)

scheduler = BackgroundScheduler(timezone="Asia/Shanghai")
scheduler.add_job(my_daily_update_task, CronTrigger(hour=4, minute=0, second=0, timezone="Asia/Shanghai"))

@asynccontextmanager
async def lifespan(app: FastAPI):
    scheduler.start()
    print("定时任务调度器已启动，设定为每天中国时间 04:00 执行。")
    yield
    scheduler.shutdown()

app = FastAPI(lifespan=lifespan)

CORRECT_USERNAME = "zyh"
CORRECT_PASSWORD = "123456"

class FundRequest(BaseModel):
    username: str
    password: str
    fund_code: str

@app.post("/fund_csv")
async def read_csv_return(data: FundRequest):
    if data.username != CORRECT_USERNAME or data.password != CORRECT_PASSWORD:
        raise HTTPException(status_code=401, detail="账号或密码错误")
    file_path = Path(get_funds_row_path()) / f"{data.fund_code}.csv"
    if not file_path.is_file():
        raise HTTPException(status_code=404, detail=f"未找到基金代码 [{data.fund_code}] 对应的 CSV 文件")
    try:
        # 异步非阻塞读取文件，瞬间释放 CPU 给予并发支撑
        async with aiofiles.open(file_path, "rb") as f:
            binary_content = await f.read()
        return Response(
            content=binary_content,
            media_type="text/csv",
            headers={"Content-Disposition": f"attachment; filename={data.fund_code}.csv"}
        )
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"读取文件出错: {str(e)}")

class CsvUploadRequest(BaseModel):
    username: str
    password: str
    csv_name: str
    csv_data: bytes

@app.post("/get_csv")
async def get_csv_stor(data: CsvUploadRequest):
    if data.username != CORRECT_USERNAME or data.password != CORRECT_PASSWORD:
        raise HTTPException(status_code=401, detail="账号或密码错误")
    funds_row_dir = Path(get_funds_row_path())
    funds_row_dir.mkdir(parents=True, exist_ok=True)
    file_path = funds_row_dir / f"{data.csv_name}.csv"
    exists = file_path.exists()
    try:
        async with aiofiles.open(file_path, "wb") as f:
            await f.write(data.csv_data)
        return {"code": 200, "message": f"成功{'更新' if exists else '创建'} CSV 文件: {data.csv_name}.csv", "path": str(file_path)}
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"保存文件失败: {str(e)}")

if __name__ == "__main__":
    uvicorn.run("file_update:app", host="127.0.0.1", port=8880, reload=True, workers=4)