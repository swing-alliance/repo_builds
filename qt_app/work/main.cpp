#include <QCoreApplication>
#include"work.h"
#include<QThread>
#include<QDebug>
#include<QtConcurrent/QtConcurrent>
#include<QFuture>
#include<QThreadPool>
#include"job.h"
#include<QMutex>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // QThread *t1 = new QThread;
    // Worker *w1 = new Worker;
    // w1->moveToThread(t1);

    // QObject::connect(t1, &QThread::started, w1, &Worker::run);
    // QObject::connect(w1, &Worker::finished, t1, &QThread::quit);
    // QObject::connect(t1, &QThread::finished, w1, &QObject::deleteLater);
    // QObject::connect(t1, &QThread::finished, t1, &QObject::deleteLater);

    // QThread *t2 = new QThread;
    // Worker *w2 = new Worker;
    // w2->moveToThread(t2);
    // QObject::connect(t2, &QThread::started, w2, &Worker::run);
    // QObject::connect(w2, &Worker::finished, t2, &QThread::quit);
    // QObject::connect(t2, &QThread::finished, w2, &QObject::deleteLater);
    // QObject::connect(t2, &QThread::finished, t2, &QObject::deleteLater);

    // t1->start();
    // t2->start();
    QFuture<int> result=QtConcurrent::run([](int a,int b){qDebug()<<QThread::currentThreadId();return a+b;},10,20);
    qDebug()<<result.result();
    int num=100;
    QMutex * lock=new QMutex;
    QThreadPool * p=new QThreadPool;
    job * j=new job(num,*lock);
    job * j1=new job(num,*lock);
    job * j2=new job(num,*lock);

    p->start(j);
    p->start(j1);
    p->start(j2);




    return a.exec();

}
