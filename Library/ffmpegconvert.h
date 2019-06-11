#ifndef FFMPEGCONVERT_H
#define FFMPEGCONVERT_H

#include <QObject>
#include "singleton.h"
#include "dystructu.h"
#include "YtbTool.h"

/*!
    FFmpeg Convert的单例类
*/

#define FFmpeg FFmpegConvert::getInstance()

class FFmpegConvert : public QObject,public Singleton<FFmpegConvert>
{
    Q_OBJECT
    friend class  Singleton<FFmpegConvert>;
    friend class QSharedPointer<FFmpegConvert>;

public:
    void ffmpegConvertClick(MEDIAINFO media,QString conFormat);     // 开始convert

    QList<CONVERTMEDIA> getConvertList();
private:
    Q_DISABLE_COPY(FFmpegConvert)
    explicit FFmpegConvert(QObject *parent = 0);
    ~FFmpegConvert();

signals:
    void sendConvertProcess(QString path,double pro);

public slots:
    void ffmpegConvertStop(MEDIAINFO media);                        // 停止convert
    void convertProcessSlot(const QString &filePath,const double &process);

private:
    YtbTool*                _ffmpegTool;
    QList<CONVERTMEDIA>     _convertList;       // convertList列表
};

#endif // FFMPEGCONVERT_H
