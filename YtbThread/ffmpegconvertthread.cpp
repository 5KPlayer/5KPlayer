#include "ffmpegconvertthread.h"
#include "globalarg.h"

#include <QDir>
#include <QProcess>
#include <qdebug.h>
FFmpegConvertThread::FFmpegConvertThread(const CONVERTMEDIA &args, QObject *parent) : YtbThread(parent)
{
    qRegisterMetaType<QList<MEDIAINFO>>("QList<MEDIAINFO>");
    _ffmpegArgs = args;
}

void FFmpegConvertThread::threadRun()
{
    QProcess pro;
    QString fileIn  = _ffmpegArgs.media.filepath;
    QString fileOut = _ffmpegArgs.outfilePath;
    QString appPath = Global->getuserYtbPaht().append("/ffmpeg.exe");
    QString fileFloder = Global->convertPath();
    QDir dir(fileFloder);
    if(!dir.exists()) {
        dir.mkpath(fileFloder);
    }

    QStringList arguments;
    if(_ffmpegArgs.conFormat == ".mp3")
        arguments<<"-hide_banner"<<"-y"<<"-i"<< fileIn <<"-ar"<< "44100" << "-b:a"<<"320K"<<"-ac"<<"2"<<"-vn"<<"-sn"<<"-f"<<"mp3"<<fileOut;
    else
        arguments<<"-hide_banner"<<"-y"<<"-i"<< fileIn <<"-c:a" <<"libfdk_aac"<< "-ar"<< "44100" << "-b:a"<<"320K"<<"-ac"<<"2"<<"-vn"<<"-sn"<<fileOut;

    pro.setProcessChannelMode(QProcess::MergedChannels);
    pro.start(appPath, arguments);


    while(pro.state() != QProcess::NotRunning)
    {
        pro.waitForFinished(300);
        if(b_isStopped)
        {
            qDebug("pro.close()");
            pro.close();
            pro.waitForFinished();
            break;
        }
        analyseProgress(pro.readAllStandardOutput());
    }
    emit convertPorcess(_ffmpegArgs.media.filepath,-1);

}

void FFmpegConvertThread::analyseProgress(const QByteArray &output)
{
    QString info = output;
    QStringList timeList;
    double proessValue = 0;
    if(info.startsWith("size=")) {
        int pos = info.indexOf("time=");
        QString time = info.mid(pos+5,8);
        timeList = time.split(":");
        int h = timeList.at(0).toInt();
        int m = timeList.at(1).toInt();
        int s = timeList.at(2).toInt();
        int dur = h*3600+m*60+s;
        proessValue = (double)dur;
    }

    emit convertPorcess(_ffmpegArgs.media.filepath,proessValue);
}
