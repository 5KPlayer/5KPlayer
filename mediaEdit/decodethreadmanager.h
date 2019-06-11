#ifndef DECODETHREADMANAGER_H
#define DECODETHREADMANAGER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QVector>
#include <QMap>
#include "decodethread.h"

class DecodeThreadManager : public QObject
{
    Q_OBJECT
public:
    explicit DecodeThreadManager(QObject *parent = 0);
    ~DecodeThreadManager();
    void push(Task *task,bool refresh = true);

signals:

public slots:
    void onThreadFinished(Task *task);
    void setItemListSize(int size);

private:
    QVector<Task*>    _getFramesThreadVec;
    QVector<Task*>    _getOneFrameVec;
    QMap<int,QVector<Task*>>   _requestFrameMap;
    QMutex            _mutex;
    int               _itemListSize;
};

#endif // DECODETHREADMANAGER_H
