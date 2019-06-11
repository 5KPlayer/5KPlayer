#include "editsavethread.h"
#include "globalarg.h"
#include <QTime>
#include <QFileInfo>
#include <QFile>
#include <QDebug>

#define PI 3.141592653

EditSaveThread::EditSaveThread(CutParams params, QObject *parent) : QThread(parent)
{
    _params = params;
    _savingStop = false;
}

void EditSaveThread::run()
{
    QString flipString;
    if(_params.flip == 0)   //无翻转
        flipString = "";
    else if(_params.flip == 1)  //水平翻转
        flipString = "hflip";
    else if(_params.flip == 2)  //垂直翻转
        flipString = "vflip";
    else                        //先水平,再垂直翻转
        flipString = "hflip,vflip";

    QTime start;
    int startSec = _params.startTime / 1000;
    start.setHMS(startSec / 3600,startSec % 3600 / 60,startSec % 60,_params.startTime % 1000);
    QString startTime = start.toString("HH:mm:ss.z");
    int64_t duration = (_params.endTime - _params.startTime) / 1000;
    QString af      = "";
    if(_params.speed != 1.0) {
        QString atempo  = "";
        if(_params.speed >= 0.5)
        {
            atempo = QString::number(_params.speed);
        } else {
            atempo = QString::number(0.5) + QString(",atempo=") + QString::number(_params.speed / 0.5);
        }
        af.append(QString("atempo=%1").arg(atempo));
    }
    if(_params.adelay != 0) {
        if(!af.isEmpty()) {
            af.append(",");
        }
        af.append(QString("adelay=%2|%2").arg(QString::number(_params.adelay)));
    }
    if(_params.volume != 1) {
        if(!af.isEmpty()) {
            af.append(",");
        }
        af.append(QString("volume=%3").arg(QString::number(_params.volume)));
    }

    QString vf = "";
    if(_params.speed != 1.0) {
        vf.append(QString("setpts=PTS/%1").arg(QString::number(_params.speed)));
        duration = duration / _params.speed;
    }
    if(_params.saturation != 1.0 || _params.contrast != 1.0 || _params.brightness != 0) {
        if(!vf.isEmpty()) {
            vf.append(",");
        }
        vf.append(QString("eq=saturation=%1:contrast=%2:brightness=%3").
                  arg(QString::number(_params.saturation)).
                  arg(QString::number(_params.contrast)).
                  arg(QString::number(_params.brightness)));
    }
    if(_params.rotate!= 0) {
        if(!vf.isEmpty()) {
            vf.append(",");
        }
        int times = _params.rotate / 90;
        if(times == 1)
            vf.append("transpose=1");
        else if(times == 2)
            vf.append("transpose=1,transpose=1");
        else
            vf.append("transpose=1,transpose=1,transpose=1");

    }
    if(!flipString.isEmpty()) {
        if(!vf.isEmpty()) {
            vf.append(",");
        }
        vf.append(flipString);
    }

    QString enCodec = "libx264";
#ifdef Q_OS_WIN
    if(Global->hasEncHardware(CardIntel,"h264")) {
        enCodec = "h264_qsv";
    } else if(Global->hasEncHardware(CardNvidia,"h264")) {
        enCodec = "h264_nvenc";
    } else if(Global->hasEncHardware(CardAMD,"h264")) {
        enCodec = "h264_amf";
    }
#else
    if(_params.resolution.width() >= 480 && _params.resolution.height() >= 480) {
        enCodec = "h264_videotoolbox";
    }
#endif

    QStringList args,codeArgs;
    args.clear();
    codeArgs.clear();
    args << "-ss" << startTime<< "-i" << _params.filePath
         << "-sn" << "-t" << QString::number(duration);
    if(!af.isEmpty()) {
        args << "-af" << af;
    }
    if(!vf.isEmpty()) {
        args << "-vf" << vf;
    }

    args << "-b:v" << getCurBitRate()
         << "-c:a" << "aac";
    codeArgs << args;
    args << "-c:v" << enCodec;
    codeArgs << "-c:v" << "libx264"
             << "-y" << _params.outFilePath;
    args << "-y" << _params.outFilePath;

    bool source = startSaveFile(args);

    if(!_savingStop) {
        // 不是使用软件编码并且保存文件失败，使用软件编码
        if(!source && enCodec != "libx264") {
            startSaveFile(codeArgs);
        }
        emit saveDone();
    }
}

void EditSaveThread::endSaving()
{
    _savingStop = true;
}

bool EditSaveThread::deleteFile()
{
    bool source = false;
    QFileInfo fileInfo(_params.outFilePath);
    if(fileInfo.exists() && fileInfo.isFile())      //文件存在
    {
        int count = 0;
        while(!source)
        {
            source = QFile::remove(_params.outFilePath);
            count++;
            if(count == 5) {
                break;
            }
            msleep(100);
        }
    }
    return source;
}

QString EditSaveThread::getCurBitRate()
{
    QString bit = QString::number(_params.bitRate);
    if(_params.bitRate == 0) {
        if(_params.resolution.width() >= 2160) {
            bit = "10000";
        } else if(_params.resolution.width() >= 1080) {
            bit = "4000";
        } else {
            bit = "2000";
        }
    }
    return bit.append("K");
}

bool EditSaveThread::startSaveFile(QStringList args)
{
    QString appPath;
#ifdef Q_OS_WIN
    appPath = Global->getuserYtbPaht().append("/ffmpeg.exe");
#else
    appPath = Global->getAppPath().append("/ffmpeg");
#endif
    QProcess pro;
    pro.setProcessChannelMode(QProcess::MergedChannels);
    pro.start(appPath,args);
    //转码文件总时长
    int cutDuration = ((_params.endTime - _params.startTime) / 1000) / _params.speed;  //截取时间间隔
    int totalSecs   = 0;
    int fps = 0;
    while(pro.state() != QProcess::NotRunning)
    {
        if(_savingStop)     //用户中断了转码
        {
            pro.close();
            deleteFile();
            break;
        }
        pro.waitForFinished(200);
        QByteArray data = pro.readAllStandardOutput();
        int pos = data.indexOf("time=");
        if(pos != -1)
        {
            //根据截取时长获取当前转码进度
            QByteArray current = data.mid(pos + 5,8);   //当前转码的时间戳
            QList<QByteArray> timeList = current.split(':');
            int h = timeList.at(0).toInt();
            int m = timeList.at(1).toInt();
            int s = timeList.at(2).toInt();
            totalSecs = h * 3600 + m * 60 + s;

        }
        int fpsPos = data.indexOf("fps=");
        if(fpsPos != -1) {
            int length = data.indexOf(" ",fpsPos + 5) - (fpsPos + 4);
            QByteArray curpts = data.mid(fpsPos + 4,length);   //当前pts
            fps = QString(curpts).toInt();
        }
        emit saveProgressAndFPS(totalSecs,cutDuration,fps);
    }
    bool saveSource = true;
    if(!_savingStop) {
        QFile file(_params.outFilePath);
        if(file.size() == 0) {
            saveSource = false;
        }
    }
    return saveSource;
}
