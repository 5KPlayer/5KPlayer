#ifndef FFMPEGCONVERTTHREAD_H
#define FFMPEGCONVERTTHREAD_H

#include <QObject>
#include "YtbThread.h"

class FFmpegConvertThread :  public YtbThread
{
    Q_OBJECT
public:
    explicit FFmpegConvertThread(const CONVERTMEDIA &args,QObject *parent = 0);

    void threadRun() Q_DECL_OVERRIDE;

private:
    void analyseProgress(const QByteArray &output);
signals:
    void convertPorcess(QString,double);
public slots:
private:
    CONVERTMEDIA _ffmpegArgs;
};

#endif // FFMPEGCONVERTTHREAD_H
