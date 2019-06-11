#include "ytbanalysubthread.h"
#include <QProcess>
#include <QDebug>
YtbAnalySubThread::YtbAnalySubThread(const AnalysisArgs &args, QObject *parent) : YtbThread(parent)
{
    m_args = args;
}

YtbAnalySubThread::~YtbAnalySubThread()
{
    qDebug("YtbAnalySubThread del");
}

void YtbAnalySubThread::threadRun()
{
    QProcess pro;
    QStringList args;

    if(m_args.https.isEmpty()) {
        args << "--list-subs" << m_args.url;
    } else {
        args << "--proxy" << m_args.https << "--list-subs" << m_args.url;
    }

    pro.start(m_args.appPath, args);

    while(pro.state() != QProcess::NotRunning)
    {
        pro.waitForFinished(500);
        if(b_isStopped)  {
            pro.close();
            break;
        }
    }

    if(!b_isStopped)
    {
        QByteArray outPut = pro.readAllStandardOutput();
        if(pro.exitCode() == 0 || !outPut.isEmpty())
        {
            analyzeSubtitles(outPut);
            return;
        }
    }
}

void YtbAnalySubThread::analyzeSubtitles(QByteArray subByte)
{
    QStringList subtitleList;
    QString subtitles = QString(subByte);
    QStringList subList = subtitles.split("\n");
    bool subFlag = false;
    foreach(QString substr, subList) {
        if(substr.contains("Available subtitles"))
            subFlag = true;
        if(subFlag && substr!="Language formats") {
            QStringList List = substr.split(",");
            if(List.size()>1) {
                QStringList subtiList = List.at(0).split(" ");
                QStringList language;
                foreach(QString langstr, subtiList) {
                    if(!langstr.isEmpty())
                        language.append(langstr);
                }

                if(language.size() >=2) {
                    subtitleList.append(language.at(0));
                }
            }
        }
    }
    emit analySubFinish(m_args.url,subtitleList);
}
