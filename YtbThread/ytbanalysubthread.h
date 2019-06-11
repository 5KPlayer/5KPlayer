#ifndef YTBANALYSUBTHREAD_H
#define YTBANALYSUBTHREAD_H

#include <QObject>
#include "YtbThread.h"

class YtbAnalySubThread : public YtbThread
{
    Q_OBJECT
public:
    explicit YtbAnalySubThread(const AnalysisArgs &args,QObject *parent = nullptr);
    ~YtbAnalySubThread();

    void threadRun() Q_DECL_OVERRIDE;
signals:
    void analySubFinish(const QString &url,const QStringList &subList);
public slots:
private:
    void analyzeSubtitles(QByteArray subByte); // 通过返回的byte信息获取字幕信息
private:
    AnalysisArgs        m_args;
};

#endif // YTBANALYSUBTHREAD_H
