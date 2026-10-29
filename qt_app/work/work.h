#ifndef WORK_H
#define WORK_H
#include <QObject>
#include <QThread>
#include <atomic>
#include<QDebug>
class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr):QObject(parent){}
    void setStop(){m_stop=true;}
    void resetStop(){m_stop=false;}
signals:
    void progress(int);
    void finished();
public slots:
    void run()
    {
        m_stop=false;
        qDebug() << "worker tid:" << QThread::currentThreadId();
        for(int i=0;i<=100;i++)
        {
            qDebug()<<QThread::currentThreadId()<<"的打印:"<<i;
            if(m_stop) break;
            emit progress(i);
            // QThread::msleep(30);
        }
        emit finished();
    }
private:
    std::atomic<bool> m_stop{false};
};
#endif // WORK_H
