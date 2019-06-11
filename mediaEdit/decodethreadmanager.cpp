#include "decodethreadmanager.h"
#include <QThreadPool>
#include <QDebug>

DecodeThreadManager::DecodeThreadManager(QObject *parent) : QObject(parent)
{
    _itemListSize = 0;
}

DecodeThreadManager::~DecodeThreadManager()
{
    foreach (Task *t, _getOneFrameVec) {
        t->stopThread();
    }
    foreach (Task *t, _getFramesThreadVec) {
        t->stopThread();
    }
    foreach (QVector<Task*> taskVec, _requestFrameMap) {
        foreach (Task *t, taskVec) {
            t->stopThread();
        }
    }
    QThreadPool::globalInstance()->waitForDone();
}

void DecodeThreadManager::push(Task *task, bool refresh)
{
    connect(task,SIGNAL(threadFinished(Task*)),this,SLOT(onThreadFinished(Task*)),Qt::DirectConnection);
    _mutex.lock();
    if(task->type() == Task::TaskDecodeOneFrame)
    {
        if(refresh)
        {
            foreach (Task *t, _getOneFrameVec) {
                t->stopThread();
            }
        }
        _getOneFrameVec.push_front(task);
    }
    else if(task->type() == Task::TaskDecodeAllFrame)
    {
        foreach (Task *t, _getFramesThreadVec) {
            t->stopThread();
        }
        _getFramesThreadVec.push_front(task);
    }
    else{
        RequestOneFrameThread *ts = (RequestOneFrameThread*)task;
        foreach (Task *t, _requestFrameMap[ts->requestIndex()]) {
            t->stopThread();
        }
        _requestFrameMap[ts->requestIndex()].push_front(task);
        for(auto it = _requestFrameMap.begin();it != _requestFrameMap.end(); ++it)
        {
            if(it.key() >= _itemListSize)
            {
                foreach (Task *t, it.value()) {
                    t->stopThread();
                }
            }
        }
    }
    _mutex.unlock();
    static int youxianji = 0;
    youxianji ++;
    QThreadPool::globalInstance()->start(task,youxianji);
}

void DecodeThreadManager::onThreadFinished(Task *task)
{
    if(task == NULL)
        return;
    _mutex.lock();
    if(task->type() == Task::TaskDecodeOneFrame)
        _getOneFrameVec.removeOne(task);
    else if(task->type() == Task::TaskDecodeAllFrame)
        _getFramesThreadVec.removeOne(task);
    else
    {
        RequestOneFrameThread *t = (RequestOneFrameThread*)task;
        _requestFrameMap[t->requestIndex()].removeOne(task);
    }
    _mutex.unlock();
}

void DecodeThreadManager::setItemListSize(int size)
{
    _itemListSize = size;
}

