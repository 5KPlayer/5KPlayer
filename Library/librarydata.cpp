#include "librarydata.h"
#include "ffmpeg/displayaspectratio.h"
#include "openfile.h"
#include "playermainwidget.h"
#include <QFileInfo>
#include <random>
#include <QDebug>
#include <QApplication>
#include <QThread>
#include <QtSql>
#include <QHostInfo>
#include "addmediathead.h"
#include "cmenu.h"
#include "delmediathead.h"
#include <shlwapi.h>
#include "sqloperation.h"
#include "configure.h"
#include "globalarg.h"
#include "ffmpegconvert.h"
#include <QLabel>
#include <QPainter>
#include <QPen>

LibraryData::LibraryData(QObject *parent)
    : QObject(parent)
{
    b_doubleClickPlay   = false;
    b_isConCPT          = false;
    b_isCPTCon          = false;
    _addListPos         = 0;
    _showMaxCount       = 6;
    _curSelectItem      = "pMovies";
    _curPlayItem        = "pMovies";
    _sortMode           = Data;

    qsrand(QDateTime::currentMSecsSinceEpoch());
    getlocalIp();
    InitData();
}

QList<MEDIAINFO> LibraryData::getmedaiData()
{
    return _videoList;
}

QList<MEDIAINFO> LibraryData::getmusicData()
{
    return _musicList;
}

QList<QString> LibraryData::getitemNames()
{
    return _itemList;
}

bool LibraryData::addItem(const QString &name)
{
    bool isAdd = true;
    for(int i=0;i<_itemList.size();i++) {
        if(QString::compare(_itemList.at(i),name,Qt::CaseInsensitive) == 0) {
            isAdd = false;
            break;
        }
    }
    if(isAdd) {
        if(Sql->addItemtoSql(name)) {
            _itemList.append(name);
        }
    }
    return isAdd;
}

void LibraryData::deItem(const QString &itemName)
{
    if(Sql->sqlDelforItemName(itemName)) {
        delGroup(itemName);
    }
    if(Sql->delItemtoSql(itemName)) {
        _itemList.removeAll(itemName);
    }

    emit upvideoList(true,false,itemName);
}

void LibraryData::addpathList(const QStringList &paths, const QString &itemName)
{
    QString item = _curSelectItem;
    if(!itemName.isEmpty())
        item = itemName;
    _addListPos = 0;
    QList<MEDIAINFO> list = getMediaforItem(item);
    AddMediaThead* addmediaThead = new AddMediaThead;
    connect(addmediaThead,SIGNAL(sendMedialist(int,QList<MEDIAINFO>)),SLOT(addmediaListSlt(int,QList<MEDIAINFO>)));
    connect(addmediaThead,SIGNAL(analyzefinish()),SIGNAL(analyzefinish()));
    connect(this,SIGNAL(stopAnalyzeFile()),addmediaThead,SLOT(cancelLoad()));
    addmediaThead->setAddpathList(paths,list,item,_showMaxCount);
    addmediaThead->start();
}

bool LibraryData::addmedia(const QString &path, const QString &itemName, bool convert)
{
    MEDIAINFO media;

    if(!checkMediaExits(path)) {
        return false;
    }
    QString curPath = QDir::toNativeSeparators(path);

    QString Pin = getfilePin(curPath);
    if (checkPicturefile(curPath))
        return false;
    // 判断是否在播放列表
    for(int i=0;i<_mediaList.size();++i) {
        if (QString::compare(_mediaList.at(i).filepath,curPath,Qt::CaseInsensitive) == 0 && _mediaList.at(i).pin == Pin) {
            if(!convert) {
                _curplaymedia = _mediaList.at(i);
                emit upplaymedia(_curplaymedia);
            } else {
                emit upvideoList(true,convert,itemName);
            }
            return true;
        }
    }

    DisplayAspectRatio* diaplay = new DisplayAspectRatio();
    diaplay->getmediainfo(curPath,&media);

    delete diaplay;

    if(itemName.isEmpty()) {
        if(media.mediaType == 1) {
            media.itemname = "pMovies";
        } else if(media.mediaType == 2) {
            media.itemname = "pMusic";
        } else {
            return false;
        }
    } else if(itemName == "ytb"){
        if(media.mediaType == 1) {
            media.itemname = "yVideos";
        } else {
            media.itemname = "yMusic";
        }
        emit upvideoList(true,convert,media.itemname);
    } else {
        if(media.mediaType != 0)
            media.itemname = itemName;
        else
            return false;
    }
    QFileInfo fileinfo(curPath);

    media.title = fileinfo.completeBaseName();
    media.ext   = fileinfo.suffix();
    media.filepath = curPath;
    media.pin = Pin;
    media.filesize = fileinfo.size();
    media.fileAddTime = getCurData();
    media.isFavorites = false;
    media.isSelect = false;

    if(!convert)
        _curplaymedia = media;

    if(Global->mirrorFiles().contains(media.ext.toLower()))
        return true;

//     if(checkUrlFile(curPath)) // 如果是网络文件，则不加入播放列表
//         return true;
    //检查是否在数据库中存在，如果存在，则不更新到数据库，只更新到media列表
    if(checkMediaisUp(&_mediaList,media)) {
        Sql->updatamediatosql(_mediaList,media);
        if(media.mediaType ==1) {
            checkMediaisUp(&_videoList,media);
        } else {
            checkMediaisUp(&_musicList,media);
        }
    } else {
        Sql->insertmediatosql(media);
        if(media.mediaType ==1) {
            QFileInfo fileinfo(media.filepath);
            Config->addVideoFormat(fileinfo.suffix());
            _videoList.prepend(media);
        } else {
            QFileInfo fileinfo(media.filepath);
            Config->addAudioFormat(fileinfo.suffix());
            _musicList.prepend(media);
        }
    }

    clearShowItemName();
    emit upvideoList(true,convert,itemName);
    return true;
}

void LibraryData::addDropMediaList(const QList<MEDIAINFO> &medialist)
{
    Sql->insertmediaListtosql(medialist);
    for(int i=0;i<medialist.count();i++) {
        _mediaList.append(medialist.at(i));
        if(medialist.at(i).mediaType == 1) {
            _videoList.append(medialist.at(i));
        } else {
            _musicList.append(medialist.at(i));
        }
    }
    clearShowItemName();
    emit upvideoList(false,false,_curSelectItem);
}

bool LibraryData::delmedia(const QList<MEDIAINFO> &medialist, bool isSelectAll, bool bup, bool ismove)
{
    QStringList pathList;
    pathList.clear();
    if(_curSelectItem == "pMovies" || _curSelectItem == "pMusic" || ismove) {
        for(int i=0;i<medialist.size();++i) {
            if(ismove)
                pathList.append(medialist.at(i).filepath);
            for(int j=_mediaList.size()-1;j>=0;--j) {
                if(_mediaList.at(j).filepath == medialist.at(i).filepath) {
                    _mediaList.removeAt(j);
                }
            }
            for(int k=_videoList.size()-1;k>=0;k--) {
                if(_videoList.at(k).filepath == medialist.at(i).filepath) {
                    _videoList.removeAt(k);
                }
            }
            for(int f=_musicList.size()-1;f>=0;f--) {
                if(_musicList.at(f).filepath == medialist.at(i).filepath) {
                    _musicList.removeAt(f);
                }
            }
        }
    } else {
        for(int i=0;i<medialist.size();++i) {
            for(int j=_mediaList.size()-1;j>=0;--j) {
                if(_mediaList.at(j).filepath == medialist.at(i).filepath && _mediaList.at(j).itemname == medialist.at(i).itemname) {
                    _mediaList.removeAt(j);
                }
            }
            for(int k=_videoList.size()-1;k>=0;k--) {
                if(_videoList.at(k).filepath == medialist.at(i).filepath && _videoList.at(k).itemname == medialist.at(i).itemname) {
                    _videoList.removeAt(k);
                }
            }
            for(int f=_musicList.size()-1;f>=0;f--) {
                if(_musicList.at(f).filepath == medialist.at(i).filepath && _musicList.at(f).itemname == medialist.at(i).itemname) {
                    _musicList.removeAt(f);
                }
            }
        }
    }
    if(ismove) {
        //delFileToRecycleDlg(pathList);
    }
    // 检查是否在convert列表
    QList<CONVERTMEDIA> conList = FFmpeg->getConvertList();

    for(int k=0;k<conList.count();k++) {
        bool isExit = false;
        for(int i=0;i<_mediaList.count();i++) {
            if(_mediaList.at(i).filepath == conList.at(k).media.filepath) {
                isExit = true;
                break;
            }
        }
        if(!isExit)
            FFmpeg->ffmpegConvertStop(conList.at(k).media);
    }

    bool isExits = false;
    if(getcurmediaPath() != "") {
        for(int i=0;i<_mediaList.size();i++) {
            if(QString::compare(_mediaList.at(i).filepath,getcurmediaPath(),Qt::CaseInsensitive) == 0 && getplayinfo().itemname == _mediaList.at(i).itemname){
                isExits = true;
                break;
            }
        }
        if(!isExits)
            PMW->stopVlc(true);
    }

    DelMediaThead* delmediaThead = new DelMediaThead;
    delmediaThead->setDelMediaInfo(medialist,_curSelectItem,isSelectAll,ismove);
    delmediaThead->start();
    emit upvideoList(bup,false,_curSelectItem);
    return false;
}

void LibraryData::upmedia(const MEDIAINFO &mediainfo, bool upList)
{
    if(Sql->upMediaFavorites(mediainfo)) {
        for(int i=_mediaList.size()-1;i>=0;--i) {
            if(QString::compare(_mediaList.at(i).filepath,mediainfo.filepath,Qt::CaseInsensitive) == 0) {
                MEDIAINFO media = _mediaList.at(i);
                media.isFavorites = mediainfo.isFavorites;
                _mediaList.replace(i,media);
            }
        }
        if(mediainfo.mediaType == 1) {
        for(int j=_videoList.size()-1;j>=0;--j) {
            if(QString::compare(_videoList.at(j).filepath , mediainfo.filepath,Qt::CaseInsensitive) == 0) {
                MEDIAINFO media = _videoList.at(j);
                media.isFavorites = mediainfo.isFavorites;
                _videoList.replace(j,media);
            }
        }
        } else {
            for(int k=_musicList.size()-1;k>=0;k--) {
                if(QString::compare(_musicList.at(k).filepath , mediainfo.filepath,Qt::CaseInsensitive) == 0) {
                    MEDIAINFO media = _musicList.at(k);
                    media.isFavorites = mediainfo.isFavorites;
                    _musicList.replace(k,media);
                }
            }
        }
    }
    if((_curSelectItem == "FMovies") || (_curSelectItem == "FMusic") || upList)
        emit upvideoList(true,false,_curSelectItem);
    else
        emit upvideoList(false,true,_curSelectItem);
}

void LibraryData::setselectitem(const QString &item)
{
    _curSelectItem = item;
}

QString LibraryData::getselectitem()
{
    return _curSelectItem;
}

void LibraryData::setplayItem(const QString &item)
{
    if(item.isEmpty())
        _curPlayItem = _curSelectItem;
    else
        _curPlayItem = item;
}

QList<MEDIAINFO> LibraryData::getMediaforItem(const QString &item)
{
    QList<MEDIAINFO> mediaList;
    if(item == "FMovies") {
        QList<MEDIAINFO> List;
        for(int i=0;i<_videoList.size();++i){
            if(_videoList.at(i).isFavorites)
                List.append(_videoList.at(i));
        }
        mediaList = movesMediaList(List);
    } else if(item == "FMusic") {
        QList<MEDIAINFO> List;
        for(int i=0;i<_musicList.size();++i){
            if(_musicList.at(i).isFavorites)
                List.append(_musicList.at(i));
        }
        mediaList =  movesMediaList(List);
    } else if (item == "yVideos"){
        for(int i=0;i<_videoList.size();++i){
            if(_videoList.at(i).itemname == item)
                mediaList.append(_videoList.at(i));
        }
    } else if (item == "yMusic"){
        for(int i=0;i<_musicList.size();++i){
            if(_musicList.at(i).itemname == item)
                mediaList.append(_musicList.at(i));
        }
    } else if(item == "pMusic") {
        QList<MEDIAINFO> List;
        for(int i=0;i<_musicList.size();++i){
            if(_musicList.at(i).itemname != "yMusic")
                List.append(_musicList.at(i));
        }
        mediaList = movesMediaList(List);
    } else if (item == "pMovies") {
        QList<MEDIAINFO> List;
        for(int i=0;i<_videoList.size();++i){
            if(_videoList.at(i).itemname != "yVideos")
                List.append(_videoList.at(i));
        }
        mediaList = movesMediaList(List);
    } else {
        for(int i=0;i<_mediaList.size();++i){
            if(_mediaList.at(i).itemname ==  item)
                mediaList.append(_mediaList.at(i));
        }
    }
    return mediaList;
}

MEDIAINFO LibraryData::getplayinfo()
{
    return _curplaymedia;
}

void LibraryData::clearPlayinfo()
{
    MEDIAINFO curpalyinfo;
    _curplaymedia = curpalyinfo;
    emit upplaymedia(_curplaymedia);
}

QString LibraryData::getcurmediaPath()
{
    return getpath(_curplaymedia);
}

bool LibraryData::isHDCurVideo()
{
    bool isHD = false;
    _StreamInfo stream = getVideoStream(_curplaymedia);
    if(stream.resolution.width() >= 1920 || stream.resolution.height() >= 1080) {
        isHD = true;
    }
    return isHD;
}

bool LibraryData::setplaymedia(const MEDIAINFO &mediainfo)
{
    if(checkMediaExits(mediainfo.filepath)) {
        _curplaymedia = mediainfo;
        return true;
    } else {
        _curplaymedia.title = "";
        return false;
    }
}

QString LibraryData::getCurMediaCodec()
{
    STREAMINFO  stream = getVideoStream(_curplaymedia);
    return stream.codec;
}

QList<MEDIAINFO> LibraryData::movesMediaList(const QList<MEDIAINFO> &List)
{
    QList<MEDIAINFO> movesList;
    movesList.clear();

    for(int i=0;i<List.size();++i) {
        if(List.at(i).itemname != "yVideos" || List.at(i).itemname != "yMusic") {
            bool isappend = true;
            if(movesList.size() > 0) {
                for(int j=0;j<movesList.size();++j) {
                    if(QString::compare(movesList.at(j).filepath,List.at(i).filepath,Qt::CaseInsensitive) == 0) {
                        isappend = false;
                        break;
                    }
                }
                if(isappend)
                    movesList.append(List.at(i));
            } else {
                movesList.append(List.at(i));
            }
        }
    }
    return movesList;
}

QList<MEDIAINFO> LibraryData::removeItemForList(QList<MEDIAINFO> &List, const MEDIAINFO &mediainfo)
{
    int index = -1;
    for(int i=0;i<List.size();++i) {
        if((QString::compare(List.at(i).filepath,mediainfo.filepath,Qt::CaseInsensitive) == 0) &&
           (List.at(i).itemname == mediainfo.itemname))
            index = i;
    }
    if(index != -1) {
        List.removeAt(index);
    }
    return List;
}

void LibraryData::setMusicSort(LibraryData::MusicSort t)
{
    _sortMode = t;
    SortMusic(&_musicList);
}

bool CapitySortMusic(const MEDIAINFO &mediaFirst, const MEDIAINFO &mediaSecond)
{
    if(LibData->getMusicSort() == LibData->Title)
        return (QString::compare(mediaFirst.title , mediaSecond.title, Qt::CaseInsensitive)) < 0;
    else if(LibData->getMusicSort() == LibData->Artist) {
        return (QString::compare(mediaFirst.artist , mediaSecond.artist, Qt::CaseInsensitive)) < 0;
    } else if(LibData->getMusicSort() == LibData->Data)
        return (mediaFirst.fileAddTime > mediaSecond.fileAddTime);
    return true;
}

void LibraryData::SortMusic(QList<MEDIAINFO> *medialist)
{
    qSort(medialist->begin(),medialist->end(),CapitySortMusic);
}

QString LibraryData::getfileSize(double fileSize, int digit)
{
    int unit = 0;
    while(fileSize>1024) {
        fileSize = fileSize/1024;
        unit++;
    }
    return QString::number(fileSize,10,digit).append(_fileUnit[unit]);
}

QString LibraryData::sectotime(int duration)
{

    if(duration <= 0)
        return "00:00:00";

    const int hour = duration/3600;
    const int minute = duration%3600/60;
    const int second = duration%3600%60;

    QString hourStr = QString::number(hour).rightJustified(2, '0');
    QString minuteStr = QString::number(minute).rightJustified(2, '0');
    QString secondStr = QString::number(second).rightJustified(2, '0');
    return QString("%1:%2:%3").arg(hourStr).arg(minuteStr).arg(secondStr);
}

QSize LibraryData::getaspectRatio(const MEDIAINFO &media)
{
    QSize size;
    QStringList aspect = getVideoStream(media).aspectRatio.split(":");
    QString width = "";
    QString height= "";
    if(aspect.count() >= 2) {
        width = aspect.at(0);
        height= aspect.at(1);
    }
    size.setWidth(width.toInt());
    size.setHeight(height.toInt());
    return size;
}

void LibraryData::setmusiclist(const QList<MEDIAINFO> &musiclist)
{
    _musicList = musiclist;
}

double LibraryData::getCurData()
{
    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString str = time.toString("yyyy:MM:dd:hh:mm:ss:zzz");
    QStringList strp = str.split(":");
    QString data;
    for(int i=0;i<strp.size();++i) {
        data.append(strp.at(i));
    }
    return data.toLongLong();
}

QString LibraryData::getfilePin(const QString &path)
{
    QFile theFile(path);
    theFile.open(QIODevice::ReadOnly);
    theFile.seek(theFile.size()/2);
    QByteArray ba = QCryptographicHash::hash(theFile.read(1000),QCryptographicHash::Md5);
    theFile.close();

    return ba.toHex().constData();
}

STREAMINFO LibraryData::getVideoStream(const MEDIAINFO &media)
{
    STREAMINFO stream;
    for(int i=0;i<media.streamList.size();++i) {
        if(media.streamList.at(i).streamType == 0 &&
           media.streamList.at(i).resolution.width() != 0 &&
           media.streamList.at(i).resolution.height()!= 0){
            stream = media.streamList.at(i);
            break;
        }
    }
    return stream;
}

STREAMINFO LibraryData::getMusicStream(const MEDIAINFO &media)
{
    STREAMINFO stream;
    for(int i=0;i<media.streamList.size();++i) {
        if(media.streamList.at(i).streamType == 1){
            stream = media.streamList.at(i);
            break;
        }
    }
    return stream;
}

void LibraryData::InitData()
{
    _videoList.clear();
    _musicList.clear();
    _mediaList.clear();
    _itemList.clear();
    _udpList.clear();
    _recent.clear();
    _bjSevList.clear();
    Sql->initData(&_urlMedia,&_mediaList,&_itemList);
    Sql->initRecent(&_recent);
    for(int i=0;i<_mediaList.size();++i) {
        if(_mediaList.at(i).mediaType == 1) {
            _videoList.append(_mediaList.at(i));
        } else {
            _musicList.append(_mediaList.at(i));
        }
    }
}

void LibraryData::InitMusic()
{
    _musicList.clear();
    Sql->sqlGetMusicMediaList(&_musicList);
}

void LibraryData::delGroup(const QString &itemName)
{

    for(int i=_mediaList.size()-1;i>=0;--i) {
        if(_mediaList.at(i).itemname == itemName) {
            _mediaList.removeAt(i);
        }
    }
    for(int i=_videoList.size()-1;i>=0;--i) {
        if(_videoList.at(i).itemname == itemName) {
            _videoList.removeAt(i);
        }
    }
    for(int i=_musicList.size()-1;i>=0;--i) {
        if(_musicList.at(i).itemname == itemName) {
            _musicList.removeAt(i);
        }
    }
}

MEDIAINFO LibraryData::getnextmedia(bool isAuto)
{
    if(_curplaymedia.filepath.isEmpty())
        return  _curplaymedia;
    QList<MEDIAINFO> media;

    media = getValidList(getMediaforItem(_curPlayItem));
    if(media.size() == 0) {
        _curplaymedia.title = "";
        upmediaPlay();
        return _curplaymedia;
    }

    int index;
    for(index=0;index<media.size();index++) {
        if(media.at(index).filepath == _curplaymedia.filepath)
           break;
    }

    switch (Global->getPlayMode()) {
    case 1:
    {
        if(index < media.size()-1)
            index++;
        else {
            _curplaymedia.title = "";
            upmediaPlay();
            return _curplaymedia;
        }
    }
        break;
    case 2:
        if(!isAuto) {
            if(index < media.size()-1)
                index++;
            else {
                _curplaymedia.title = "";
                upmediaPlay();
                return _curplaymedia;
            }
        }
        break;
    case 3:
    {
        if(index >= media.size()-1)
            index = 0;
        else
            index++;
    }
        break;
    case 4:
        index = qrand()% (media.size());
        break;
    default:
        break;
    }

    _curplaymedia = media.at(index);
    upmediaPlay();
    return _curplaymedia;
}

MEDIAINFO LibraryData::getprevmedia(bool isAuto)
{
    if(_curplaymedia.filepath.isEmpty())
        return  _curplaymedia;
    QList<MEDIAINFO> media;

    media = getValidList(getMediaforItem(_curPlayItem));
    if(media.size() == 0) {
        _curplaymedia.title = "";
        upmediaPlay();
        return _curplaymedia;
    }

    int index;
    for(index=0;index<media.size();index++) {
        if(media.at(index).filepath == _curplaymedia.filepath)
           break;
    }

    switch (Global->getPlayMode()) {
    case 1:
    {
        if(index > 0)
            index--;
        else {
            _curplaymedia.title = "";
            upmediaPlay();
            return _curplaymedia;
        }
    }
        break;
    case 2:
        if(!isAuto) {
            if(index > 0)
                index--;
            else {
                _curplaymedia.title = "";
                upmediaPlay();
                return _curplaymedia;
            }
        }
        break;
    case 3:
    {
        if(index == 0)
            index = media.size()-1;
        else
            index--;
    }
        break;
    case 4:
        index = qrand()% (media.size());
        break;
    default:
        break;
    }

    _curplaymedia = media.at(index);
    upmediaPlay();
    return _curplaymedia;
}

bool LibraryData::checkMediaExits(const QString &mediaPath)
{
    QFile file(mediaPath);
    return file.exists();
}

QList<MEDIAINFO> LibraryData::getValidList(const QList<MEDIAINFO> &media)
{
    QList<MEDIAINFO> valMedia;
    valMedia.clear();
    for(int i=0;i<media.size();++i) {
        if(checkMediaExits(media.at(i).filepath))
            valMedia.append(media.at(i));
    }
    return valMedia;
}

QString LibraryData::getpath(const MEDIAINFO &media)
{
    return QDir::toNativeSeparators(media.filepath);
}

//bool LibraryData::delFileToRecycleDlg(QStringList pathList)
//{
//    SHFILEOPSTRUCT  shDelFile;
//    memset(&shDelFile,0,sizeof(SHFILEOPSTRUCT));
//    int filelength = 0;
//    foreach(QString path, pathList) {
//        if(_curplaymedia.filepath == path)
//            PMW->stopVlc(false);
//        filelength+=path.size();
//    }

//    TCHAR szFile[filelength+pathList.size()+1] = {0};
//    int szfilePos = 0;
//    for(int i=0;i<pathList.size();++i) {
//        if(i>0)
//            szfilePos+=pathList.at(i-1).size()+1;
//        lstrcpy(szFile+szfilePos,(LPCSTR) pathList.at(i).utf16());
//    }

//    // 设置SHFILEOPSTRUCT的参数为删除做准备
//    shDelFile.wFunc = FO_DELETE;       // 执行的操作
//    shDelFile.pFrom = szFile;        // 操作的对象，也就是目录
//    shDelFile.pTo = NULL;          // 必须设置为NULL
//    shDelFile.fFlags = FOF_ALLOWUNDO|FOF_NOCONFIRMATION|FOF_FILESONLY;    //删除到回收站

//    return SHFileOperation(&shDelFile);    //删除
//}

bool LibraryData::checkFormat(const QString &formatList, const QString &format)
{
    QString curformat = "*.";
    curformat.append(format.toLower());
    return formatList.contains(curformat);
}

bool LibraryData::checkUrlFile(QString path)
{
    bool isUrl = true;
    if(path.size() > 2) {
        if(path.mid(1,1) == ":") {
            isUrl = false;
        }
    }
    return isUrl;
}

void LibraryData::addmediaListSlt(int count, const QList<MEDIAINFO> &list)
{
    if(list.size()<=0)
        return;
    if(count == 0) {
        for(int i=0;i<list.size();++i){
            if(list.at(i).mediaType == 1) {
                _videoList.insert(_addListPos,list.at(i));
            } else {
                _musicList.insert(_addListPos,list.at(i));
            }
            _mediaList.insert(_addListPos,list.at(i));
            _addListPos++;
        }
        emit upvideoList(true,false,_curSelectItem);
        emit upAnalyzeInfo(list);
    } else {
        for(int i=0;i<list.size();++i){
            if(list.at(i).mediaType == 1) {
                _videoList.insert(_addListPos,list.at(i));
            } else {
                _musicList.insert(_addListPos,list.at(i));
            }
            _mediaList.insert(_addListPos,list.at(i));
            _addListPos++;
        }
       emit upvideoList(false,false,_curSelectItem);
       emit upAnalyzeInfo(list);
    }
}

void LibraryData::searchChange(const QString &search)
{
    _searchInfo = search;
    clearShowItemName();
    emit upvideoList(true,false,"");
}

void LibraryData::addNetList(NetInfo udpInfo)
{
    bool isAdd = true;
    foreach (NetInfo clientInfo, _udpList) {
        if(clientInfo.ipAddr == udpInfo.ipAddr){
            isAdd = false;
            break;
        }
    }
    if(isAdd) {
        _udpList.append(udpInfo);
    }
    //emit upUdpClientList();
}

void LibraryData::removeNet(QString udpIp)
{
    for(int i=_udpList.size()-1;i>=0;--i) {
        if(_udpList.at(i).ipAddr == udpIp) {
            _udpList.removeAt(i);
            break;
        }
    }
//    foreach (NetInfo clientInfo, _udpList) {
//        if(clientInfo.ipAddr == udpIp){
//            _udpList.removeOne(clientInfo);
//            break;
//        }
//    }
    //emit upUdpClientList();
}

void LibraryData::addBjSev(QZeroConfService *service)
{
    if(checkIsLocalIp(service->ip.toString()))
        return;
    bool isAdd = true;
    foreach (QZeroConfService *serviceInfo, _bjSevList) {
        if(serviceInfo->ip == service->ip){
            isAdd = false;
            break;
        }
    }

    if(isAdd) {
        _bjSevList.append(service);
    }
    emit upUdpClientList();
}

void LibraryData::delBjSev(QZeroConfService *service)
{
    _bjSevList.removeOne(service);
    emit upUdpClientList();
}

void LibraryData::airDisConnect()
{
    b_isConCPT = false;
    _udpClient.ip.clear();
    _udpClient.name = "";
    _udpClient.domain = "";
    _udpClient.host = "";
    _udpClient.port = -1;
    _udpClient.type = "";
    emit upWidgetAirPlay(false);
}

void LibraryData::setUdpClient(QZeroConfService* netInfo)
{
    b_isConCPT = true;
    _udpClient.ip = netInfo->ip;
    _udpClient.name = netInfo->name;
    _udpClient.domain = netInfo->domain;
    _udpClient.host = netInfo->host;
    _udpClient.port = netInfo->port;
    _udpClient.type = netInfo->type;
    emit upWidgetAirPlay(true);
}

QZeroConfService LibraryData::getUdpClient()
{
    return _udpClient;
}

void LibraryData::setUdpServer(NetInfo netInfo)
{
    b_isCPTCon = true;
    _udpServer.ipAddr = netInfo.ipAddr;
    _udpServer.machineName = netInfo.machineName;
    _udpServer.port = netInfo.port;
}

void LibraryData::clearUdpServer()
{
    b_isCPTCon = false;
    _udpServer.ipAddr = "";
    _udpServer.machineName = "";
    _udpServer.port = 0;
}

NetInfo LibraryData::getUdpServer()
{
    return _udpServer;
}

void LibraryData::getlocalIp()
{
    _localIps.clear();
    QHostInfo info = QHostInfo::fromName(QHostInfo::localHostName());
    info.addresses();//QHostInfo的address函数获取本机ip地址
    //如果存在多条ip地址ipv4和ipv6：
    foreach(QHostAddress address,info.addresses())
    {
        if(address.protocol()==QAbstractSocket::IPv4Protocol){//只取ipv4协议的地址
            _localIps << address.toString();
        }
    }
}

bool LibraryData::checkIsLocalIp(const QString &Ip)
{
    bool ret = false;
    foreach (QString localIp, _localIps) {
        if(localIp == Ip) {
            ret = true;
            break;
        }
    }
    return ret;
}

void LibraryData::disScreenprotection()
{
    bool bAct;
    ::SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &bAct, 0);
    if (bAct)
    {
        ::SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, false, 0, 0);
    }
    ::SetThreadExecutionState( ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED );
}

void LibraryData::recScreenprotection()
{
    ::SystemParametersInfo( SPI_SETSCREENSAVEACTIVE, true, 0, 0 );
    ::SetThreadExecutionState( ES_CONTINUOUS );
}

void LibraryData::setSearchText(QString searchText)
{
    _searchInfo = searchText;
}

bool LibraryData::isConstItem(QString item)
{
    bool isConstItem = false;
    for(int i=0;i<11;i++) {
        if(QString::compare(Global->_constItemName[i],item,Qt::CaseInsensitive) == 0) {
            isConstItem = true;
            break;
        }
    }
    return isConstItem;
}

QImage LibraryData::getSelPictures()
{
    // 这里是必然有宽高的
    QSize mediaSize = getVideoStream(selmedialist.at(0)).resolution;
    int w,h;
    if(mediaSize.height() == 0)
        w = 90*Global->_screenRatio;
    else
        w = 90*Global->_screenRatio*mediaSize.width()/mediaSize.height();
    h = 90*Global->_screenRatio;;

    QImage image,imageTemp;
    if(selmedialist.at(0).screenshot.isEmpty()) {
        if(selmedialist.at(0).mediaType == 1) {
            imageTemp.load(":/res/png/videoshot.png");
        } else {
            imageTemp.load(":/res/png/musicshot.png");
        }
    } else {
        imageTemp.loadFromData(selmedialist.at(0).screenshot);
    }

    image = imageTemp.scaled(w,h,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPainter painter(&image) ;

    int edistance = 5*Global->_screenRatio;
    int imageHeigh = 25*Global->_screenRatio;
    int listCount = selmedialist.size();

    //改变画笔和字体
    QFont font;
    font.setPixelSize(imageHeigh/5*3);
    font.setFamily(Global->getFontFamily());

    int imageWidth = 25*Global->_screenRatio;
    QLabel label;
    int textWidth = label.fontMetrics().width(QString::number(listCount));
    int textSize  = QString::number(listCount).size();
    double textfloat = textWidth/textSize;

    imageWidth += textfloat*(textSize-1)*2;
    QPen pen;
    pen.setColor( QColor(255, 255, 255));
    painter.setPen(pen);
    pen.setWidth(0);
    painter.setFont(font);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setBrush(QBrush(QColor(255,0,0),Qt::SolidPattern));
    painter.setPen(QColor(255,0,0));
    painter.drawEllipse(edistance,edistance,imageWidth,imageHeigh);

    QRectF one(edistance+imageWidth/4,edistance+imageHeigh/4,imageWidth/2,imageHeigh/2);
    painter.drawRect(one);

    painter.setPen(Qt::white);
    painter.drawText(one,Qt::AlignCenter,QString::number(listCount));

    return image;
}

bool LibraryData::selectItemIsDrop()
{
    const QString _sNoDropItemName = "FMovies FMusic yVideos yMusic pAirRecord";
    bool isDrop = false;
    if(_sNoDropItemName.contains(_curSelectItem)) {
        isDrop = true;
    }
    return isDrop;
}

double LibraryData::analyzeProess(QByteArray ba)
{
    QString info = ba;
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
    return proessValue;
}

bool LibraryData::checkMediaisUp(QList<MEDIAINFO> *mediaList, const MEDIAINFO &media)
{
    bool isUp = false;
    for(int i=0;i<mediaList->size();++i) {
        if(mediaList->at(i).filepath == media.filepath) {
            if(mediaList->at(i).pin != media.pin) {
                isUp = true;
                MEDIAINFO tempMedia = media;
                tempMedia.itemname = mediaList->at(i).itemname;
                mediaList->removeAt(i);
                mediaList->prepend(tempMedia);
            }
        }
    }
    if(!isUp)
        mediaList->prepend(media);
    return isUp;
}

bool LibraryData::checkMediaisAirPlay(const MEDIAINFO &media)
{
    _StreamInfo streamInfo;
    bool ret = false;
    if(media.mediaType == 1)
        streamInfo = getVideoStream(media);
    else
        streamInfo = getMusicStream(media);
    QFileInfo fileinfo(media.filepath);
    if((fileinfo.suffix().toUpper() == "MP4" && streamInfo.codec.toUpper() == "H264") || media.mediaType == 2) {
        ret = true;
    }
    return ret;
}

void LibraryData::upmediaPlay()
{
    emit upplaymedia(_curplaymedia);
}

void LibraryData::cleanUpList()
{
    QList<MEDIAINFO> delList;
    delList.clear();
    for(int i=_mediaList.count()-1;i>=0;--i) {
        if(!checkMediaExits(_mediaList.at(i).filepath)) {
            delList.append(_mediaList.at(i));
            _mediaList.removeAt(i);
        }
    }
    for(int i=_videoList.count()-1;i>=0;--i) {
        if(!checkMediaExits(_videoList.at(i).filepath)) {
            _videoList.removeAt(i);
        }
    }
    for(int i=_musicList.count()-1;i>=0;--i) {
        if(!checkMediaExits(_musicList.at(i).filepath)) {
            _musicList.removeAt(i);
        }
    }
    DelMediaThead* delmediaThead = new DelMediaThead;
    delmediaThead->setDelMediaInfo(delList,"",false,false);
    delmediaThead->start();
    upvideoList(true,false,"");
}

void LibraryData::mediaListMove(const MEDIAINFO &media1, const MEDIAINFO &media2)
{
    int index = -1;
    int index1= -1;
    for(int i=0;i<_mediaList.count();++i) {
        if(_mediaList.at(i).filepath == media1.filepath && _mediaList.at(i).itemname == media1.itemname){
            index = i;
        }
        if(_mediaList.at(i).filepath == media2.filepath && _mediaList.at(i).itemname == media2.itemname) {
            index1 = i;
        }
    }
    if(index != -1 && index1!= -1) {
        _mediaList.move(index,index1);
    }
    _videoList.clear();
    _musicList.clear();
    for(int i=0;i<_mediaList.size();++i) {
        if(_mediaList.at(i).mediaType == 1) {
            _videoList.append(_mediaList.at(i));
        } else if(_mediaList.at(i).mediaType == 2){
            _musicList.append(_mediaList.at(i));
        }
    }
}

QList<MEDIAINFO> LibraryData::getItemShowList()
{
    QList<MEDIAINFO> tempList;
    QList<MEDIAINFO> retList;
    tempList = getMediaforItem(_curSelectItem);
    for (int i=0;i<tempList.size();++i){
        if(tempList.at(i).title.contains(_searchInfo,Qt::CaseInsensitive) || tempList.at(i).artist.contains(_searchInfo,Qt::CaseInsensitive))
            retList.append(tempList.at(i));
    }
    return retList;
}

MEDIAINFO LibraryData::getNextShow(int showcount)
{
    MEDIAINFO mediainfo;
    if(showcount<getItemShowList().size()){
        mediainfo = getItemShowList().at(showcount);
    }
    return mediainfo;
}

void LibraryData::setMaxCount(int maxCount,bool up)
{
    _showMaxCount = maxCount;
    if(up) {
        emit upresize();
    }
}

bool LibraryData::checkPicturefile(const QString &pathList)
{
    QFileInfo file(pathList);
    return _pictureFormat.contains(file.suffix());
}

void LibraryData::clearFileInItemName(QStringList *pathList)
{
    for(int i=pathList->size()-1;i>=0;--i) {
        QFileInfo fileinfo(pathList->at(i));
        if(_curSelectItem == "pMovies") {
            if(!Config->getUserVideoFormat().contains(fileinfo.suffix().toLower()))
                pathList->removeAt(i);
        } else if(_curSelectItem == "pMusic") {
            if(!Config->getUserAudioFormat().contains(fileinfo.suffix().toLower()))
                pathList->removeAt(i);
        }
    }
}

QStringList LibraryData::getFilePathonFolder(const QString &folderpath)
{
    QStringList pathList;

    QDir dir(folderpath);
    dir.setSorting(QDir::Name);
    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);


    QFileInfoList list = dir.entryInfoList();
    for(int i = 0; i != list.size(); ++i)
    {
        if(list.at(i).isDir()) {
            if(checkFolder(list.at(i).absoluteFilePath())) {
                pathList.append(getFilePathonFolder(list.at(i).absoluteFilePath()));
            }
        } else if (list.at(i).isFile()) {
            pathList.append(list.at(i).absoluteFilePath());
        }
    }
    return pathList;
}

QStringList LibraryData::getSubFilePathOnFolder(const QString &folderpath, bool &autodetect)
{
    QStringList pathList;
    QStringList filters;

    filters << "*.srt" << "*.ass"<< "*.ssa"<< "*.aqt"<< "*.sub"<< "*.json" << "*.vtt";//设置过滤类型
    QDir dir(folderpath);
    dir.setSorting(QDir::Name);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setNameFilters(filters);//设置文件名的过滤

    int fuzzy        = Global->autodetectFuzzy();

    QFileInfoList list = dir.entryInfoList();
    for(int i = 0; i < list.size(); ++i)
    {
        QFileInfo subFile(list.at(i).absoluteFilePath());
        QFileInfo playFile(getplayinfo().filepath);
        if (list.at(i).isFile()) {
            switch (fuzzy) {
            case 0:
            case 1:
                // 只判断前面的字符，不自动选择
                if(subFile.completeBaseName().startsWith(playFile.completeBaseName()))
                    pathList.append(list.at(i).absoluteFilePath().replace("/","\\"));
                autodetect = false;
                break;
            case 2:
            case 3:
                // 只判断前面的字符，自动选择
                if(subFile.completeBaseName().startsWith(playFile.completeBaseName()))
                    pathList.append(list.at(i).absoluteFilePath().replace("/","\\"));
                autodetect = true;
                break;
            case 4:
                // 精确比较，自动选择
                if(QString::compare(subFile.completeBaseName(),playFile.completeBaseName(),Qt::CaseInsensitive) == 0) {
                    pathList.append(list.at(i).absoluteFilePath().replace("/","\\"));
                }
                autodetect = true;
                break;
            default:
                break;
            }
        }
    }
    if(pathList.count() == 0) {
        autodetect = false;
    }
    return pathList;
}

void LibraryData::setRootPath(const QString &rootpath)
{
    _rootPath = rootpath;
}

bool LibraryData::checkFolder(const QString &folderpath)
{
    int rootCount   = _rootPath.split("/").size();
    int folderCount = folderpath.split("/").size();
    if(folderCount-rootCount>FOLDERDEPTH)
        return false;
    else
        return true;
}

QStringList LibraryData::getValidFilePath(const QStringList &pathList)
{
    QStringList validPathList;
    QString videoFormat = Config->getUserVideoFormat();
    QString musicFormat = Config->getUserAudioFormat();
    foreach(QString path, pathList) {
        QFileInfo fileinfo(path);
        if(!checkFormat(_pictureFormat,fileinfo.suffix())\
           && (checkFormat(musicFormat,fileinfo.suffix())|| checkFormat(videoFormat,fileinfo.suffix()))
           /*&& !checkUrlFile(path)*/) {
            validPathList.append(path);
        }
    }
    return validPathList;
}

QString LibraryData::getVideoAspectRatio(const QString &FolderPath)
{
    QDir dir(FolderPath);

    QFileInfoList list = dir.entryInfoList(QDir::Files|QDir::NoDotAndDotDot,QDir::Name);

    for(int i = 0; i != list.size(); ++i)
    {
        MEDIAINFO media;
        QString curPath = QDir::toNativeSeparators(list.at(i).absoluteFilePath());

        DisplayAspectRatio* diaplay = new DisplayAspectRatio();
        if(diaplay->getmediainfo(curPath,&media) == -1) {
            continue;
        }
        if(media.mediaType == 1) {

            STREAMINFO stream = getVideoStream(media);
            return stream.aspectRatio;
        }
    }
    return "";
}

QList<MEDIAINFO> LibraryData::getUrlMedia()
{
    return _urlMedia;
}

void LibraryData::upUrlMediaList(const QList<MEDIAINFO> mediaList)
{
    QList<MEDIAINFO> delSqlList;
    delSqlList.clear();
    // 先替换lib中的数据

    for(int j=0;j<mediaList.size();j++) {
        for(int i=0;i<_urlMedia.size();++i) {
            if(_urlMedia.at(i).webUrl == mediaList.at(j).webUrl) {
                delSqlList.append(_urlMedia.at(i));
                _urlMedia.replace(i,mediaList.at(j));
                break;
            }
        }
    }
    Sql->updataUrlmediatoSql(delSqlList,mediaList);
}

bool SortResolution(const STREAMINFO &formatFirst, const STREAMINFO &formatSecond)
{
    return (formatFirst.resolution.height() > formatSecond.resolution.height());
}

void LibraryData::sortResolution(QList<STREAMINFO> *streamList)
{
    qSort(streamList->begin(),streamList->end(),SortResolution);
}

void LibraryData::sortFormat(QList<STREAMINFO> *videoformat)
{
    QString format = Global->_urlFormat[Global->downloadFormat()];
    int resoult = QString(Global->_urlResolution[Global->downloadResolution()]).toInt();

    bool isExitFormat = false;  // 优先选择的格式是否存在
    for(int i=0;i<videoformat->size();++i) {
        if(QString(videoformat->at(i).ext).toLower() == format.toLower()) {
            isExitFormat = true;
            break;
        }
    }
    // 如果优先选中格式存在
    QList<STREAMINFO> selFormat,otherFormat;
    if(isExitFormat) {
        for(int i=0;i<videoformat->size();++i) {
            if(QString(videoformat->at(i).ext).toLower() == format.toLower()) {
                selFormat.append(videoformat->at(i));
            } else {
                otherFormat.append(videoformat->at(i));
            }
        }
        sortResolution(&selFormat); // 把优先选择的格式按照分辨率排序
        sortResolution(&otherFormat);
        int resoultPos = -1;

        STREAMINFO selResolution;
        for(int i=0;i<selFormat.size();++i) {
            if(selFormat.at(i).resolution.height() == resoult) {
                resoultPos = i;
                selResolution = selFormat.at(i);
                selFormat.removeAt(i);
                break;
            }
        }
        if(resoultPos != -1)
            selFormat.prepend(selResolution);
        selFormat.append(otherFormat);
    } else {
        for(int i=0;i<videoformat->size();++i) {
            if(videoformat->at(i).resolution.height() == resoult) {
                selFormat.append(videoformat->at(i));
            } else {
                otherFormat.append(videoformat->at(i));
            }
        }
        sortResolution(&selFormat); // 把优先选择的格式按照分辨率排序
        sortResolution(&otherFormat);

        selFormat.append(otherFormat);
    }

    videoformat->clear();
    for(int i=0;i<selFormat.size();++i) {
        videoformat->append(selFormat.at(i));
    }
}

bool LibraryData::checkYtbDeploy()
{
    bool isExits = false;
    QString folderpath = Global->getuserYtbPaht().append("/youtube_dl");
    QString filepath = Global->getuserYtbPaht().append("/youtube_dl/__init__.py");
    QFileInfo folder(folderpath);
    QFileInfo file(filepath);
    if(folder.isDir()) {
        isExits = file.isFile();
    }
    return isExits;
}

void LibraryData::addRecent(const QString &filePath)
{
    // 如果存在于历史列表中，则先移除列表
//    if(checkUrlFile(filePath))
//        return;
    QString path = QDir::toNativeSeparators(filePath);
    for(int i=0;i<_recent.size();++i) {
        if(QString::compare(_recent.at(i),path,Qt::CaseInsensitive) == 0) {
            _recent.removeAt(i);
            Sql->removeRecent(path);
            break;
        }
    }

    bool isRemove = true;
    if(_recent.size() >= RECENTCOUNT) {
        isRemove = Sql->removeRecent(_recent.at(RECENTCOUNT-1));
        _recent.removeAt(RECENTCOUNT-1);
    }

    _recent.prepend(path);
    if(isRemove)
        Sql->addRecent(path);
    Menu->initRecentFiles();
}

void LibraryData::clearRecent()
{
    _recent.clear();
    Sql->clearRecent();
}

QStringList LibraryData::getRecent()
{
    return _recent;
}

