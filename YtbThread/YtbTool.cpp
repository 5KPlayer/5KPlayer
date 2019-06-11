#include "YtbTool.h"
#include "YtbAnalysisThread.h"
#include "YtbDownloadThread.h"
#include "ytbanalysubthread.h"
#include "ffmpegconvertthread.h"
#include <QThreadPool>
#include <QtDebug>

YtbTool::YtbTool(QObject *p) : QObject(p)
{
    qRegisterMetaType<QList<MEDIAINFO>>("QList<MEDIAINFO>");

    p_analySourceThread    = new QThreadPool(this);
    p_analySubtitleThread  = new QThreadPool(this);
    p_downloadSourceThread = new QThreadPool(this);
    p_FFmpegConvertThread  = new QThreadPool(this);

    m_analySourceThreadHash.clear();
    m_analySubtitleThreadHash.clear();
    m_downloadSourceThreadHash.clear();
    m_FFmpegConvertThreadHash.clear();
}

YtbTool::~YtbTool()
{
    p_analySourceThread->clear();
    p_analySubtitleThread->clear();
    p_downloadSourceThread->clear();
    p_FFmpegConvertThread->clear();

    QList<QString> keys1 = m_analySourceThreadHash.keys();
    QList<QString> keys2 = m_analySubtitleThreadHash.keys();
    QList<QString> keys3 = m_downloadSourceThreadHash.keys();
    QList<QString> keys4 = m_FFmpegConvertThreadHash.keys();

    foreach (QString url, keys1) {
        stopThread(url, AnalySource);
    }

    foreach (QString url, keys2) {
        stopThread(url, AnalySubtitle);
    }

    foreach (QString url, keys3) {
        stopThread(url, DownloadSource);
    }
    foreach (QString url, keys4) {
        stopThread(url, FFmpegCon);
    }
}

void YtbTool::analysisUrl(const AnalysisArgs &analyArgs)
{
    if(!m_analySourceThreadHash.contains(analyArgs.url)) {
        YtbThread *thread = new YtbAnalysisThread(analyArgs);
        m_analySourceThreadHash.insert(analyArgs.url, thread);
        connect(thread, SIGNAL(analysisFinish(QString,QList<MEDIAINFO>)),SLOT(analysisFinishSlot(QString,QList<MEDIAINFO>)));
        p_analySourceThread->start(thread);
    }
}

void YtbTool::analysisSub(const AnalysisArgs &analyArgs)
{
    if(!m_analySubtitleThreadHash.contains(analyArgs.url)) {
        YtbThread *thread = new YtbAnalySubThread(analyArgs);
        connect(thread,SIGNAL(analySubFinish(QString,QStringList)),SLOT(analySubFinishSlot(QString,QStringList)));
        m_analySubtitleThreadHash.insert(analyArgs.url, thread);
        p_analySubtitleThread->start(thread);
    }
}


void YtbTool::stopThread(const QString &url, const Type &type)
{
    YtbThread *thread = NULL;
    QThreadPool *threadPool = NULL;
    switch (type) {
    case AnalySource:
    {
        if(!m_analySourceThreadHash.contains(url))
            return;
        threadPool = p_analySourceThread;
        thread = m_analySourceThreadHash.value(url);
        thread->disconnect();
        m_analySourceThreadHash.remove(url);
        break;
    }
    case AnalySubtitle:
    {
        if(!m_analySubtitleThreadHash.contains(url))
            return;
        threadPool = p_analySubtitleThread;
        thread = m_analySubtitleThreadHash.value(url);
        thread->disconnect();
        m_analySubtitleThreadHash.remove(url);
        break;
    }
    case FFmpegCon:
    {
        if(!m_FFmpegConvertThreadHash.contains(url))
            return;
        threadPool = p_FFmpegConvertThread;
        thread = m_FFmpegConvertThreadHash.value(url);
        thread->disconnect();
        m_FFmpegConvertThreadHash.remove(url);
        break;
    }
    }
    if(thread == NULL || threadPool == NULL) return;

    switch (thread->ststus()) {
    case NotRun:
        threadPool->cancel(thread);
        thread->deleteLater();
        thread = NULL;
        break;
    case Running:
        thread->stop();
        break;
    case Finished:
        thread->deleteLater();
        thread = NULL;
        break;
    }
}

void YtbTool::ffmpegConvert(const CONVERTMEDIA &ffmpegArgs)
{
    if(!m_analySourceThreadHash.contains(ffmpegArgs.media.filepath)) {
        YtbThread *thread = new FFmpegConvertThread(ffmpegArgs);
        m_FFmpegConvertThreadHash.insert(ffmpegArgs.media.filepath, thread);
        connect(thread, SIGNAL(convertPorcess(QString,double)),SLOT(ffmpegConProcessSlot(QString,double)));
        p_FFmpegConvertThread->start(thread);
    }
}

void YtbTool::analysisFinishSlot(const QString &url, const QList<MEDIAINFO> &mediaList)
{
    m_analySourceThreadHash.remove(url);
    emit analysisFinish(url, mediaList);
}

void YtbTool::downFinishSlot(const QString &url, const QString &fileName)
{
    m_downloadSourceThreadHash.remove(url);
    emit downFinish(url, fileName);
}

void YtbTool::downFailSlot(const QString &url)
{
    m_downloadSourceThreadHash.remove(url);
    emit downFail(url);
}

void YtbTool::analySubFinishSlot(const QString &url, const QStringList &subList)
{
    m_analySubtitleThreadHash.remove(url);
    emit analysisSubFinish(url,subList);
}

void YtbTool::ffmpegConProcessSlot(const QString &filePath,const double &process)
{
    if(process == -1)
        m_FFmpegConvertThreadHash.remove(filePath);
    emit ffmpegConProgress(filePath,process);
}
