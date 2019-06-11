#include "dlnaanalythead.h"
#include "ffmpeg/displayaspectratio.h"
#include "librarydata.h"
#include "sqloperation.h"
#include <QFileInfo>
#include <QDebug>
DLnaAnalyThead::DLnaAnalyThead(QObject *parent)
    : QThread(parent)
{
    qRegisterMetaType<QList<dlnaLib>>("QList<dlnaLib>");
}

void DLnaAnalyThead::setAddpathList(QStringList pathlist,int showMax)
{
    _pathList.clear();
    _pathList = pathlist;
    _maxCount = showMax;
}

void DLnaAnalyThead::analyzeMedia()
{
    QList<dlnaLib> list;
    QList<dlnaLib> DBlist;
    bool bShow = false;
    int sendNum = 0;
    _iscancel = false;

    for(int i=0;i<_pathList.size();i++) {
        if(_iscancel)
            break;
        dlnaLib dlnaInfo = analyzepath(_pathList.at(i));
        if(!dlnaInfo.fileName.isEmpty())  {
            list.append(dlnaInfo);
            DBlist.prepend(dlnaInfo);
        }
        if(list.size() == sendCount && !bShow) {
            emit sendDLnalist(0,list);
            sendNum+=list.size();
            if(sendNum>_maxCount)
                bShow = true;
            list.clear();
        } else if (list.size() == sendCount && bShow) {
            emit sendDLnalist(list.size(),list);
            list.clear();
        }
    }

    if(!bShow) {
        emit sendDLnalist(0,list);
    } else {
        emit sendDLnalist(list.size(),list);
    }
    Sql->addDLnaListToSql(DBlist);
    emit analyzefinish();
}

dlnaLib DLnaAnalyThead::analyzepath(const QString &path)
{
    MEDIAINFO media;
    dlnaLib dlnaInfo;
    DisplayAspectRatio* diaplay = new DisplayAspectRatio();
    diaplay->getmediainfo(path,&media);

    if(media.mediaType == 0)
        return dlnaInfo;

    QFileInfo fileinfo(path);
    dlnaInfo.fileAddTime    = LibData->getCurData();
    dlnaInfo.filePath       = path;
    dlnaInfo.fileName       = fileinfo.completeBaseName();
    dlnaInfo.fileType       = media.mediaType;
    dlnaInfo.folderType     = 3;
    dlnaInfo.fileExt        = fileinfo.suffix();
    dlnaInfo.fileCodec      = "";
    if(media.mediaType == 1) {
        dlnaInfo.fileCodec = LibData->getVideoStream(media).codec;
    } else {
        dlnaInfo.fileCodec = LibData->getMusicStream(media).codec;
    }
    dlnaInfo.fileshot       = media.screenshot;
    if(!dlnaInfo.fileshot.isEmpty())
        dlnaInfo.fileAsp        = LibData->getVideoStream(media).aspectRatio;

    return dlnaInfo;
}

void DLnaAnalyThead::cancelLoad()
{
    _iscancel = true;
}

void DLnaAnalyThead::run()
{
    analyzeMedia();
}
