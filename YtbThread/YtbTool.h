#ifndef YTBTOOL_H
#define YTBTOOL_H

#include <QObject>
#include <QHash>
#include "singleton.h"
#include "dystructu.h"
class YtbThread;
class QThreadPool;

enum Type{AnalySource, AnalySubtitle, DownloadSource,FFmpegCon};

class YtbTool : public QObject
{
    Q_OBJECT
public:
    explicit YtbTool(QObject *p=Q_NULLPTR);
    ~YtbTool();

    void analysisUrl(const AnalysisArgs &analyArgs); // 分析url
    void analysisSub(const AnalysisArgs &analyArgs); // 分析字幕
    void stopThread(const QString &url, const Type &type);  //移除url
    void ffmpegConvert(const CONVERTMEDIA &ffmpegArgs); //开始convert

private slots:
    void analysisFinishSlot(const QString &url, const QList<MEDIAINFO> &mediaList);     // 分析成功
    void downFinishSlot(const QString &url,const QString &fileName);                    // 下载成功
    void downFailSlot(const QString &url);                                              // 下载失败
    void analySubFinishSlot(const QString &url, const QStringList &subList);            // 字幕分析成功

    void ffmpegConProcessSlot(const QString &filePath,const double &process);

signals:
    void analysisFinish(const QString &url, const QList<MEDIAINFO> &mediaList);     // 分析结果
    void analysisSubFinish(const QString &url,const QStringList &subList);          // 分析字幕

    void downFinish(const QString &url,const QString &fileName);                    // 下载完成
    void downFail(const QString &url);                                              // 下载失败
    void downprogress(const QString &url, const double &process);                   // 下载进度

    void ffmpegConProgress(const QString &filePath,const double &process);

private:
    QHash<QString, YtbThread*> m_analySourceThreadHash; //分析资源
    QHash<QString, YtbThread*> m_analySubtitleThreadHash; //分析字幕
    QHash<QString, YtbThread*> m_downloadSourceThreadHash; //下载资源
    QHash<QString, YtbThread*> m_FFmpegConvertThreadHash; //Convert

    QThreadPool *p_analySourceThread;
    QThreadPool *p_analySubtitleThread;
    QThreadPool *p_downloadSourceThread;
    QThreadPool *p_FFmpegConvertThread;
};

#endif // YTBTOOL_H
