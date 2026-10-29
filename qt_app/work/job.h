#ifndef JOB_H
#define JOB_H
#include<QRunnable>
#include<QMutex>
class job :public QRunnable
{
public:
    void run() override;
    int& num;
    QMutex& lock;
    job(int& num,QMutex& lock);
};

#endif // JOB_H
