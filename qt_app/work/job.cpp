#include "job.h"
#include<QThread>
#include<QDebug>
#include<QMutex>
void job::run()
{

    while(1){
        lock.lock();
        if(num==0){return;}
        qDebug()<<"卖出第"<<num<<"个";
        num--;
        qDebug()<<"job的:"<<QThread::currentThreadId();
        lock.unlock();
        QThread::msleep(100);
    }

}

job::job(int &num, QMutex &lock)
    : num(num), lock(lock)
{
}
