#ifndef YTBDOWNLOADTHREAD_H
#define YTBDOWNLOADTHREAD_H

#include <QObject>
#include "YtbThread.h"

//没有运行过
//运行结束
//正在运行


class YtbDownloadThread : public YtbThread
{
    Q_OBJECT
public:
    explicit YtbDownloadThread(const DownloadArgs &args, QObject *parent = nullptr);
    ~YtbDownloadThread();

    void threadRun() Q_DECL_OVERRIDE;

private:
    void analyseProgress(const QByteArray &output);
    QString outputFormat();
    int searchPos(const QString &source,const int &inPos,const QString &ch,const bool &direction);  // 在source中查找从inpos 开始ch的位置，
    QString getWinFileName(QString title);          // 获取window环境下正确的文件名，在windows下。文件命名不能带有\/:*?"<>|
    QString getBaseExt(QString ext);                // 偶尔出现末尾是一个\的情况

signals:
    void progress(const QString &url, const double &process);
    void downfinish(const QString &url,const QString fileName);
    void downfail(const QString &url);
private:
    DownloadArgs    m_downArgs;
    bool            m_downFinish;
    QString         m_outputFileName;
    QString         m_ext;
};

#endif // YTBDOWNLOADTHREAD_H
