#ifndef YTBTHREAD_H
#define YTBTHREAD_H

#include <QObject>
#include <QRunnable>
#include "dystructu.h"

enum ThreadStatues{NotRun, Running, Finished};

class YtbThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit YtbThread(QObject *parent = nullptr);
    virtual ~YtbThread();

    virtual void run();
    virtual void stop();

    virtual void threadRun() = 0;

    ThreadStatues ststus() const;

signals:
    void finished();

protected:
    bool b_isRunning;
    int  i_runTimes;//运行次数
    bool b_del;
};

#endif // YTBTHREAD_H
