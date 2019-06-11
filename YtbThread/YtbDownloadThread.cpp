#include "YtbDownloadThread.h"
#include <QtDebug>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QTextCodec>
YtbDownloadThread::YtbDownloadThread(const DownloadArgs &args, QObject *parent) : YtbThread(parent)
{
    m_downArgs          = args;
    m_downFinish        = false;
    m_outputFileName    = "";
    m_ext               = "";
}

YtbDownloadThread::~YtbDownloadThread()
{
    qDebug("YtbDownloadThread del");
}

void YtbDownloadThread::threadRun()
{
    QProcess pro;
    QStringList cmdArgs;

    if(!m_downArgs.https.isEmpty()) {
        cmdArgs << "--proxy"                        //指定代理
                << m_downArgs.https;                 //代理地址与端口
    }
    cmdArgs << m_downArgs.downInfo.webUrl
            << "-i"                                    //发生错误时继续
            << "-f"                                    //指定下载的格式id
            << m_downArgs.downInfo.formatID;            //格式id
//            << "--add-metadata"
//            << "--xattrs"
//            << "--prefer-ffmpeg"                       //允许使用ffmpeg
//            << "--ffmpeg-location"                     //指定ffmpeg地址
//            << m_downArgs.ffmpegPath;              //ffmpeg地址

//    if(m_pDownloadItem->outputFormat.toLower() == "mp4"
//            || m_pDownloadItem->outputFormat == "m4a"
//            || m_pDownloadItem->outputFormat == "mp3")
//    {
//        cmdArgs << "--embed-thumbnail";                     //嵌入缩略图
//    }

        cmdArgs << "-o"                                    //指定输出路径
                << outputFormat();                         //输出路径
                //<< "Movie%(autonumber)s.%(ext)s";

    if(!m_downArgs.sub.isEmpty())
    {
        QString subString = "";
            for(int i=0;i<m_downArgs.sub.size();++i) {
                if(i == m_downArgs.sub.size()-1)
                    subString.append(m_downArgs.sub.at(i));
                else
                    subString.append(m_downArgs.sub.at(i)).append(",");
            }
        cmdArgs << "--write-sub"                           //允许写字幕
                << "--sub-lang"                           //指定要下载的字幕语言
                << subString;
//                << "--convert-subs"                        //转换字幕格式
//                << "srt"                                   //字幕输出格式
//                << "--embed-subs";                         //合并字幕文件到video
    }

    pro.start(m_downArgs.appPath, cmdArgs);

    while(pro.state() != QProcess::NotRunning)
    {
        pro.waitForFinished(300);
        if(b_isStopped)
        {
            pro.close();
            break;
        }

        analyseProgress(pro.readAllStandardOutput());
        QString errStr = pro.readAllStandardError();
        if(!errStr.isEmpty())
        {
            qWarning() <<"11111:" << errStr;
        }
    }

    if(!m_downFinish) {
        emit downfail(m_downArgs.downInfo.webUrl);
    } else {

        QString path = m_downArgs.outputPath;
        path.append("/").append(m_outputFileName);
        if(QFileInfo::exists(path)) {
            emit downfinish(m_downArgs.downInfo.webUrl,path);
        } else {
            emit downfail(m_downArgs.downInfo.webUrl);
        }
    }
}

void YtbDownloadThread::analyseProgress(const QByteArray &output)
{
    QString ext = "";
    QString outStr = QString::fromLocal8Bit(output);
    if(outStr.isEmpty())
    {
        return;
    }
    if(outStr.contains("already been downloaded")
    || outStr.contains("Correcting container in")
    || outStr.contains("Merging formats into")
    || outStr.contains("[download] 100%"))
    {
        m_downFinish = true;
        int pos;
        QString outFileName;
        if(outStr.contains("Merging formats into")) {
            int inPos = outStr.indexOf("Merging formats into");
            pos = searchPos(outStr,inPos,"Deleting original file",true);
            outFileName = outStr.mid(inPos+22,pos-inPos-24);
            QFileInfo file(outFileName);
            ext = file.suffix();
            m_ext = getBaseExt(ext.simplified());
        } else if(outStr.contains("Correcting container in")) {
            pos = outStr.indexOf("[ffmpeg] Correcting container in");
            QFileInfo fileinfo(outStr.mid(pos+34,outStr.size()-(pos+36)));
            outFileName = fileinfo.fileName();
            ext = fileinfo.suffix();
            m_ext = getBaseExt(ext.simplified());
        } else if(outStr.contains("already been downloaded")){
            int inPos = outStr.indexOf(" has already been downloaded");
            pos = searchPos(outStr,inPos,"[download]",false);
            outFileName = outStr.mid(pos+11,inPos-pos-11);
            QFileInfo file(outFileName);
            ext = file.suffix();
            m_ext = getBaseExt(ext.simplified());
        }
        if(!m_ext.isEmpty())
            m_outputFileName.append(".").append(m_ext);
    } else {
        QStringList strlist = outStr.split(" ");
        QStringList valList;
        double process = 0;
        foreach (QString val, strlist) {
            if(!val.isEmpty())
                valList.append(val);
        }
        if(valList.at(0) == "\r[download]") {
            QString pro = valList.at(1);
            QString num = pro.left(pro.size()-1);
            process = num.toDouble();
        }
        emit progress(m_downArgs.downInfo.webUrl,process);
    }
}

QString YtbDownloadThread::outputFormat()
{
    QString outputDirPath = m_downArgs.outputPath;
    QString outputFilePath = QString("%1\\%2")
            .arg(outputDirPath)
            .arg(getWinFileName(m_downArgs.downInfo.title));
    int index = 2;

    while(QFileInfo(QString("%1.%2").arg(outputFilePath).arg(m_downArgs.downInfo.ext))
          .exists())
    {
        outputFilePath = QString("%1\\%2_%3")
                .arg(outputDirPath)
                .arg(getWinFileName(m_downArgs.downInfo.title))
                .arg(index);
        ++index;
    }
    if(m_downArgs.downInfo.formatID.contains("+"))
    {
        outputFilePath = QString("%1.%2").arg(outputFilePath).arg(m_downArgs.downInfo.ext);
    } else {
        for(int i=0;i<m_downArgs.downInfo.streamList.size();i++) {
            if(m_downArgs.downInfo.streamList.at(i).streamId == m_downArgs.downInfo.formatID) {
                m_ext = m_downArgs.downInfo.streamList.at(i).ext;
                break;
            }
        }
        if(m_ext.contains("m4a"))
            outputFilePath = QString("%1.%2").arg(outputFilePath).arg("m4a");
        else {
            outputFilePath = QString("%1.%2").arg(outputFilePath).arg(m_ext);
        }
    }

    m_outputFileName = QFileInfo(outputFilePath).completeBaseName();
    return outputFilePath;
}

int YtbDownloadThread::searchPos(const QString &source, const int &inPos, const QString &ch, const bool &direction)
{
    int sLength = source.size();
    int cLength = ch.size();
    int pos = 0;
    //direction == true 从前向后查找
    if(direction) {
        for(int i=inPos;i<sLength;i++) {
            QString midStr = source.mid(i,cLength);
            if(midStr == ch) {
                pos = i;
                break;
            }
        }
    } else {
        for(int i=(inPos-cLength);i>=0;i--) {
            QString midStr = source.mid(i,cLength);
            if(midStr == ch) {
                pos = i;
                break;
            }
        }
    }
    return pos;
}

QString YtbDownloadThread::getWinFileName(QString title)
{
    QByteArray Win = title.toUtf8();
    QString winStr = "\\/:*?\"<>|";

    for(int i=0;i<winStr.length();i++) {
        Win.replace(winStr.at(i), "");
    }
    return Win;
}

QString YtbDownloadThread::getBaseExt(QString ext)
{
    return ext.replace("\"","");
}
