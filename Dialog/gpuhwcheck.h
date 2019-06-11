#ifndef GPUHWCHECK_H
#define GPUHWCHECK_H

#include "dystructu.h"

#include <QThread>
class QProcess;
class GpuHwCheck : public QThread
{
    Q_OBJECT
public:
    explicit GpuHwCheck(QObject *parent = 0);

    void setGpuInfos(const QStringList &gpuinfos);

private:
    QString getfilePath(const QString &file);
    int checked(QProcess *p_pro, QStringList &args);

signals:
    void sendGpuHwInfo(const QList<GPUHWINFO> &);

private:
    const QList<QString> qsvHWcodec {"h264_qsv","mpeg2_qsv","vc1_qsv","hevc_qsv"};
    const QList<QString> cuvidHWcodec {"h264_cuvid","mpeg2_cuvid","vc1_cuvid","hevc_cuvid"};
    const QList<QString> filePath {"/h264.MOV","/mpeg2.mpg","/vc1.mkv","/hevc.mp4"};

    QStringList _gpuInfos;

protected:
     void run();
};

#endif // GPUHWCHECK_H
