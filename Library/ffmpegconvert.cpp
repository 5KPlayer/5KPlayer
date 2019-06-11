#include "ffmpegconvert.h"
#include "globalarg.h"
#include "librarydata.h"

FFmpegConvert::FFmpegConvert(QObject *parent)
    : QObject(parent)
{
    _ffmpegTool = new YtbTool(this);

    connect(_ffmpegTool,SIGNAL(ffmpegConProgress(QString,double)),SLOT(convertProcessSlot(QString,double)));
}

FFmpegConvert::~FFmpegConvert()
{

}

void FFmpegConvert::ffmpegConvertClick(MEDIAINFO media, QString conFormat)
{
    QString filepath = Global->convertPath();
    filepath.append("/").append(media.title).append(conFormat);

    CONVERTMEDIA convertArgs;
    convertArgs.media           = media;
    convertArgs.conFormat       = conFormat;
    convertArgs.outfilePath     = filepath;
    _convertList.append(convertArgs);
    _ffmpegTool->ffmpegConvert(convertArgs);
}

QList<CONVERTMEDIA> FFmpegConvert::getConvertList()
{
    return _convertList;
}

void FFmpegConvert::ffmpegConvertStop(MEDIAINFO media)
{
    _ffmpegTool->stopThread(media.filepath,FFmpegCon);
    for(int i=0;i<_convertList.count();i++) {
        if(_convertList.at(i).media.filepath == media.filepath) {
            _convertList.removeAt(i);
            break;
        }
    }
}

void FFmpegConvert::convertProcessSlot(const QString &filePath, const double &process)
{
    int conPos = -1;
    for(int i=0;i<_convertList.count();i++) {
        if(_convertList.at(i).media.filepath == filePath) {
            conPos = i;
            break;
        }
    }

    if(conPos != -1) {
        if(process == -1) {
            if(!_convertList.at(conPos).outfilePath.isEmpty())
                LibData->addmedia(_convertList.at(conPos).outfilePath,"pMusic",true);
            _convertList.removeAt(conPos);
        } else {
            CONVERTMEDIA conMedia;
            conMedia.media      = _convertList.at(conPos).media;
            conMedia.outfilePath= _convertList.at(conPos).outfilePath;
            conMedia.proValue   = process;
            _convertList.replace(conPos,conMedia);
        }
        emit sendConvertProcess(filePath,process);
    }
}

