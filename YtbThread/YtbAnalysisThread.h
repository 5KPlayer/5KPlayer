#ifndef YTBANALYSISTHREAD_H
#define YTBANALYSISTHREAD_H

#include <QObject>
#include "YtbThread.h"



class YtbAnalysisThread : public YtbThread
{
    Q_OBJECT
public:
    explicit YtbAnalysisThread(const AnalysisArgs &args, QObject *parent = nullptr);
    ~YtbAnalysisThread();

    void threadRun() Q_DECL_OVERRIDE;

signals:
    void analysisFinish(const QString &url, const QList<MEDIAINFO> &mediaList);
private:
    QList<MEDIAINFO>    analysisJsonByte(const QByteArray &jsonByte);
    MEDIAINFO           analysisJsonObj(const QJsonObject &jsonObj);      // 解析json对象信息
    QByteArray          getPicture(const QString &szUrl);           // 获取图片
    void                removeInvalidByte();

private:
    AnalysisArgs        m_args;
    QList<QByteArray>   m_json;
};

#endif // YTBANALYSISTHREAD_H
