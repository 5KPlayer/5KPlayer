#include "YtbThread.h"

YtbThread::YtbThread(QObject *parent) : QObject(parent)
{
    i_runTimes = 0;
    b_isRunning = false;
    b_del = false;
    this->setAutoDelete(false);//不自动析构
}

YtbThread::~YtbThread()
{
    b_del = true;
}

void YtbThread::run()
{
    if(b_del) return;

    ++i_runTimes;
    b_isRunning = true;

    threadRun();

    b_isRunning = false;

    this->deleteLater();
}

void YtbThread::stop()
{
    b_isRunning = false;
}

ThreadStatues YtbThread::ststus() const
{
    if(i_runTimes == 0)
        return NotRun;
    if(i_runTimes > 0 && b_isRunning)
        return Running;
    return Finished;
}
