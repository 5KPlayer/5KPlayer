#include "addmediathead.h"
#include "ffmpeg/displayaspectratio.h"
#include "librarydata.h"
#include <QFileInfo>
#include <QDir>
#include <QMetaType>
#include "sqloperation.h"
#include <QDateTime>

AddMediaThead::AddMediaThead(QObject *parent)
    : QThread(parent)
{
    qRegisterMetaType<QList<MEDIAINFO>>("QList<MEDIAINFO>");
}

void AddMediaThead::setAddpathList(QStringList pathlist, QList<MEDIAINFO> list, QString itemname, int maxCount)
{
    _itemMedia.clear();
    _pathList = pathlist;
    _itemMedia= list;
    _itemName = itemname;
    _maxCount = maxCount;
}

void AddMediaThead::analyzeMedia()
{
    QList<MEDIAINFO> list;
    QList<MEDIAINFO> DBlist;
    bool bShow = false;
    int sendNum = 0;
    _iscancel = false;
    _ytbMediaList.clear();
    _ytbMediaList = LibData->getMediaforItem("yVideos");
    _ytbMediaList.append(LibData->getMediaforItem("yMusic"));

    for(int i=0;i<_pathList.size();i++) {
        if(_iscancel)
            break;
        MEDIAINFO media = analyzepath(_pathList.at(i));
        if(!media.title.isEmpty())  {
            list.append(media);
            DBlist.prepend(media);
        }
        if(list.size() == sendCount && !bShow) {
            emit sendMedialist(0,list);
            sendNum+=list.size();
            if(sendNum>_maxCount)
                bShow = true;
            list.clear();
        } else if (list.size() == sendCount && bShow) {
            emit sendMedialist(list.size(),list);
            list.clear();
        }
    }

    if(!bShow) {
        emit sendMedialist(0,list);
    } else {
        emit sendMedialist(list.size(),list);
    }
    Sql->insertmediaListtosql(DBlist);
    emit analyzefinish();

}

MEDIAINFO AddMediaThead::analyzepath(const QString &path)
{
    MEDIAINFO media;
    QString curPath = QDir::toNativeSeparators(path);
    QString Pin     = LibData->getfilePin(curPath);
    for(int i=0;i<_ytbMediaList.size();++i) {
        if(QString::compare(_ytbMediaList.at(i).filepath,curPath,Qt::CaseInsensitive) == 0 && _ytbMediaList.at(i).pin == Pin)
            return media;
    }
    for(int j=0;j<_itemMedia.size();++j) {
        if(QString::compare(_itemMedia.at(j).filepath,curPath,Qt::CaseInsensitive) == 0 && _itemMedia.at(j).pin == Pin){
            return media;
        }
    }

    DisplayAspectRatio* diaplay = new DisplayAspectRatio();
    diaplay->getmediainfo(curPath,&media);

    if(media.mediaType == 0)
        return media;
    // 音乐不能添加进movies
    if(media.mediaType == 2 && _itemName=="pMovies")
        return media;
    // 视频不能添加进music
    if(media.mediaType == 1 && _itemName=="pMusic")
        return media;

    QFileInfo fileinfo(curPath);
    media.title = fileinfo.completeBaseName();
    media.ext   = fileinfo.suffix();
    media.itemname = _itemName;
    media.pin = Pin;
    media.filepath = curPath;
    media.filesize = fileinfo.size();
    media.fileAddTime = LibData->getCurData();
    media.isFavorites = false;
    media.isSelect = false;
    return media;
}

void AddMediaThead::cancelLoad()
{
    _iscancel = true;
}

void AddMediaThead::run()
{
    analyzeMedia();
}
