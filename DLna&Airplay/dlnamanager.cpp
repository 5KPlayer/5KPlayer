#include "dlnamanager.h"
#include "sqloperation.h"

#include "librarydata.h"
#include "dlnaanalythead.h"
#include "z_httpserver.h"
#include <QDebug>
#include <QFileInfo>
#include <QUrl>
#include <QDesktopServices>
#include <QDir>
#include "controlmanager.h"
#include "language.h"
#include "globalarg.h"

DLNAManager::DLNAManager(QObject *parent)
{
    initDlnaData();
    _maxRows    = 0;
    _maxCols    = 0;
    _language   = Global->getLanguage();
    qRegisterMetaType<deviceInfo>("deviceInfo");
    connect(CONTROL,SIGNAL(sendGetDeviceSup(deviceInfo)),SLOT(sendGetDeviceSupSlt(deviceInfo)));
}

DLNAManager::~DLNAManager()
{
}

void DLNAManager::addOrDelVirFolder(QString name, bool actState)
{
    if(name.isEmpty())
        return;
    if(actState) {
        dlnaLib virLib;
        virLib.folderType   = 2;
        virLib.fileName     = name;
        virLib.fileAddTime  = LibData->getCurData();
        if(Sql->addDLnaInfoToSql(virLib)) {
            _dlnaData.prepend(virLib);
            emit virFolderChange(virLib,true);
            addVirFolder(name);
            emit upShareLeft(true);
        }
    } else {
        int remPos = -1;
        for(int i=0;i<_dlnaData.count();i++) {
            if(_dlnaData.at(i).fileName == name
               && _dlnaData.at(i).filePath == "") {
                remPos = i;
                break;
            }
        }
        if(remPos != -1) {
            Sql->removeDLnaInfo(name,"");
            emit virFolderChange(_dlnaData.at(remPos),false);
            _dlnaData.removeAt(remPos);
            removeVirFolder(name);
            emit upShareLeft(true);
        }
    }
    emit upShareWidget(true);
}

void DLNAManager::addOrDelFolder(QString path, bool actState)
{
    if(path.isEmpty())
        return;
    QFileInfo file(path);
    QString curPath = QDir::fromNativeSeparators(path);
    // 添加文件夹

    bool isChang = false;
    if(actState) {
        dlnaLib virLib;
        virLib.folderType   = 1;
        virLib.fileName     = file.baseName();
        virLib.filePath     = curPath;
        virLib.fileAddTime  = LibData->getCurData();
        if(Sql->addDLnaInfoToSql(virLib)) {
            _dlnaData.prepend(virLib);
            isChang = true;
            emit upShareLeft(true);
        }
    } else {
        Sql->removeDLnaInfo(file.baseName(),curPath);
         int remPos = -1;
         for(int i=0;i<_dlnaData.count();i++) {
             if(_dlnaData.at(i).fileName == file.baseName()
                && _dlnaData.at(i).filePath == curPath) {
                 remPos = i;
                 break;
             }
         }
         if(remPos != -1) {
             _dlnaData.removeAt(remPos);
             isChang = true;
             emit upShareLeft(true);
         }
    }
    if(isChang)
        emit upShareWidget(true);
}

void DLNAManager::addFilePaths(QStringList paths)
{
    _addLibPos  = 0;
    int showMax = _maxRows*_maxCols;
    DLnaAnalyThead* dlnaAnalyThead = new DLnaAnalyThead;
    connect(dlnaAnalyThead,SIGNAL(sendDLnalist(int,QList<dlnaLib>)),SLOT(addDLnaListSlt(int,QList<dlnaLib>)));
    connect(dlnaAnalyThead,SIGNAL(analyzefinish()),SIGNAL(analyzefinish()));
    connect(this,SIGNAL(stopAnalyzeFile()),dlnaAnalyThead,SLOT(cancelLoad()));
    dlnaAnalyThead->setAddpathList(paths,showMax);
    dlnaAnalyThead->start();
}

QList<dlnaLib> DLNAManager::getDLnaData()
{
    return _dlnaData;
}

bool DLNAManager::isShsreDLna(QString name)
{
    bool isShare = false;
    for(int i=0;i<_dlnaData.count();i++) {
        if(_dlnaData.at(i).folderType == 2 && _dlnaData.at(i).fileName == name) {
            isShare = true;
            break;
        }
    }
    return isShare;
}

bool DLNAManager::libInfoCompar(dlnaLib info, dlnaLib info1)
{
    bool compar = false;
    if(info.fileName == info1.fileName
            && info.filePath == info1.filePath
            && info.folderType == info1.folderType) {
        compar = true;
    }
    return compar;
}

bool DLNAManager::libInfoSupper(dlnaLib info)
{
    bool supper = true;
    if(info.fileType == 1) {
        if(QString::compare(info.fileCodec,"H264",Qt::CaseInsensitive) != 0 ||
           QString::compare(info.fileExt,"mp4",Qt::CaseInsensitive) != 0) {
            supper = false;
        }
    } else {
        if(QString::compare(info.fileCodec,"aac",Qt::CaseInsensitive) != 0 &&
           QString::compare(info.fileExt,"mp3",Qt::CaseInsensitive) != 0) {
            supper = false;
        }
    }
    return supper;
}

void DLNAManager::removeDataInSel()
{
    if(_selDlnaData.count() < 1)
        return;
    for(int i=0;i<_selDlnaData.count();i++) {
        int remPos = -1;
        for(int j=0;j<_dlnaData.count();j++) {
            if(libInfoCompar(_selDlnaData.at(i),_dlnaData.at(j))) {
                remPos = j;
                break;
            }
        }
        if(remPos!=-1) {
            if(_dlnaData.at(remPos).folderType != 3) {
            }
            if(_dlnaData.at(remPos).folderType == 2) { // 只有虚拟目录需要发该消息
                emit virFolderChange(_dlnaData.at(remPos),false);
                for(int k=0;k<_virFolderList.count();k++) {
                    if(_virFolderList.at(k)->fileName == _dlnaData.at(remPos).fileName) {
                        break;
                    }
                }
            }
            _dlnaData.removeAt(remPos);
        }
    }
    Sql->delDLnaListToSql(_selDlnaData);
    emit upShareLeft(true);
    emit upShareWidget(true);
    _selDlnaData.clear();
}

void DLNAManager::removeSelForData(dlnaLib info)
{
    int remPos = -1;
    for(int i=0;i<_selDlnaData.count();i++) {
        if(libInfoCompar(_selDlnaData.at(i),info)) {
            remPos = i;
            break;
        }
    }
    if(remPos != -1)
        _selDlnaData.removeAt(remPos);
}

bool DLNAManager::pathIsExitsData(QString path)
{
    bool isExits = false;
    for(int i=0;i<_dlnaData.count();i++) {
        if(_dlnaData.at(i).filePath == path) {
            isExits = true;
            break;
        }
    }
    return isExits;
}

QStringList DLNAManager::pathsIsExitsData(QStringList paths)
{
    QStringList reList;
    reList.clear();
    for(int i=0;i<paths.count();i++) {
        if(!pathIsExitsData(paths.at(i))) {
            reList.append(paths.at(i));
        }
    }
    return reList;
}

void DLNAManager::openFolderPath(QString path)
{
    QString url = "file:///";
    url.append(path);
    QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
}

void DLNAManager::openFilePath(QString path)
{
    my_openpathClick(path);
}

void DLNAManager::openVirFolder(QString name)
{
    emit virFolderDClick(name);
}

void DLNAManager::initServer()
{
    _virFolderList.clear();
    if(!Global->getDLnaUserIp().isEmpty()) {
        BrisaMediaServer::setIp(Global->getDLnaUserIp());
    }

    if(_dlnaData.count() > 0) {
        for(int i=0;i<_dlnaData.count();i++) {
            if(_dlnaData.at(i).folderType == 2) {
                addVirFolder(_dlnaData.at(i).fileName);
            }
        }
    }
}

QString DLNAManager::getCurItem(QString item)
{
    QString title = item;
    if(item == "yVideos") {
        if(_language == "en") {
            title = QString("(YTB)").append(ELang("LibLeft/Movies"));
        } else {
            title = QString("(YTB)").append(JLang("LibLeft/Movies"));
        }

    } else if(item == "yMusic") {
        if(_language == "en") {
            title = QString("(YTB)").append(ELang("LibLeft/Music"));
        } else {
            title = QString("(YTB)").append(JLang("LibLeft/Music"));
        }
    }
    return title;
}

CFileAtt *DLNAManager::createVirDir(QString name)
{
    CFileAtt* att = new CFileAtt;
    QString title = getCurItem(name);
    att->fileName = title;
    att->type = CFileType::Dir;
    att->id = "";
    att->parentId = "0";
    return att;
}

CFileAtt *DLNAManager::createVirFile(MEDIAINFO media,CFileAtt* parent)
{
    QFileInfo file(media.filepath);
    CFileAtt* att = new CFileAtt;
    att->fileName = file.completeBaseName();
    att->size     = file.size();
    att->path     = media.filepath;
    if(media.mediaType == 1)
        att->type = CFileType::Video;
    else
        att->type = CFileType::Audio;

    int id = 1;
    if(parent->subFiles.size() > 0) {
        QString idStr = parent->subFiles.last()->id.split(__TAB).last();
        id = idStr.toInt() + 1;
    }

    att->id = parent->id + __TAB + QString::number(id);
    att->parentId = parent->id;
    const QString hash = ZHttp->addFile(media.filepath);
    att->url = ZHttp->getHttp() + "/" + hash;
    return att;
}

void DLNAManager::addVirFolder(QString name)
{
    CFileAtt* att = nullptr;
    foreach (CFileAtt* t, _virFolderList) {
        QString title = getCurItem(name);

        if(t->fileName == title)
        {
            att = t;
            break;
        }
    }
    if(!att)
    {
        att = createVirDir(name);
        QList<MEDIAINFO> mediaList = LibData->getMediaforItem(name);
        for(int i=0;i<mediaList.count();i++) {
            CFileAtt* file = createVirFile(mediaList.at(i),att);
            att->subFiles.append(file);
        }
        _virFolderList.append(att);
    }
}

void DLNAManager::removeVirFolder(QString name)
{
    QString title = getCurItem(name);
    for(int i=0;i<_virFolderList.count();i++) {
        if(_virFolderList.at(i)->fileName == title) {
            break;
        }
    }
}

void DLNAManager::initDlnaData()
{
    QList<dlnaLib> tempList;
    tempList.clear();
    Sql->initDLnaInfo(&tempList);
    if(tempList.count() > 0) {
        // 先放入文件夹
        for(int i=0;i<tempList.count();i++) {
            if(tempList.at(i).folderType != 3) {
                _dlnaData.prepend(tempList.at(i));
            } else {
                _dlnaData.prepend(tempList.at(i));
            }
        }
    }
}

bool DLNAManager::isExitDlnaData(dlnaLib lib)
{
    bool isExits = false;
    for(int i=0;i<_dlnaData.count();i++) {
        if(_dlnaData.at(i).fileName == lib.fileName &&
           _dlnaData.at(i).filePath == lib.filePath) {
            isExits = true;
            break;
        }
    }
    return isExits;
}

void DLNAManager::addVirFile(MEDIAINFO media, QString item)
{
    QString title = getCurItem(item);
    for(int i=0;i<_virFolderList.count();i++) {
        if(_virFolderList.at(i)->fileName == title) {
            CFileAtt *att = createVirFile(media,_virFolderList.at(i));
            _virFolderList.at(i)->subFiles.append(att);
            break;
        }
    }
}

void DLNAManager::delVirFile(MEDIAINFO media, QString item)
{
    QString title = getCurItem(item);
    for(int i=0;i<_virFolderList.count();i++) {
        if(_virFolderList.at(i)->fileName == title) {
            for(int j=0;j<_virFolderList.at(i)->subFiles.count();j++) {
                if(_virFolderList.at(i)->subFiles.at(j)->path == media.filepath) {
                   _virFolderList.at(i)->subFiles.removeAt(j);
                   break;
                }
            }
        }
    }
}

void DLNAManager::delVirFolder(QString name)
{
    int delPos = -1;
    QString title = getCurItem(name);
    for(int i=0;i<_virFolderList.count();i++) {
        if(_virFolderList.at(i)->fileName == title) {
            delPos = i;
            break;
        }
    }
    if(delPos != -1) {
        _virFolderList.removeAt(delPos);
    }
}

QString DLNAManager::getVirTextInfo(QLabel *lab, QString text)
{
    QString showText = "";
    int LabWidth   = lab->width();
    int nameSize   = text.size();
    int nameLength = lab->fontMetrics().width(text);
    if(LabWidth<nameLength) {
       for(int i=0;i<nameSize;i++) {
           showText = text.left(i).append("...");
           int showWidth = lab->fontMetrics().width(showText);
           if(showWidth > LabWidth) {
               showText = text.left(i-1).append("...");
               break;
           }
       }
    } else {
       showText = text;
    }
    return showText;
}
// line 是表示拆分为几行
QString DLNAManager::getCurShowText(QLabel *lab, QString text, QString spl, int line)
{
    int labWidth = lab->width();
    QString showText = "";
    // 英语要单词分拆
    if(Global->getLanguage() == "en" || Global->getLanguage() == "zh") {
        QStringList textList = text.split(spl);
        QString temp = "";
        int index = 0;
        for(int i=0;i<textList.count();i++) {
            temp.append(textList.at(i)).append(spl);
            int tempWidth = lab->fontMetrics().width(temp);
            if(tempWidth > labWidth) {
                index++;
                if(index == line) {
                    showText.remove(i-2,2);
                    showText.append("...");
                    break;
                } else {
                    temp = "";
                    showText.append("\n");
                }
            }
            showText.append(textList.at(i)).append(" ");
        }
    } else if(Global->getLanguage() == "jp"){
        QString temp = "";
        int index = 0;
        for(int i=0;i<text.length();i++) {
            temp.append(text.at(i));
            int tempWidth = lab->fontMetrics().width(temp);
            if(tempWidth > labWidth) {
                index++;
                if(index == line) {
                    showText.remove(i-2,2);
                    showText.append("...");
                    break;
                } else {
                    temp = "";
                    showText.append("\n");
                }
            }
            showText.append(text.at(i));
        }
    }
    return showText;
}

void DLNAManager::setShareMaxRows(int max,bool isUp)
{
    _maxRows = max;
    if(isUp)
        upShareWidget(true);
}

int DLNAManager::getShareMaxRows()
{
    return _maxRows;
}

void DLNAManager::setShareMaxCols(int max)
{
    _maxCols = max;
}

int DLNAManager::getShareMaxCols()
{
    return _maxCols;
}

void DLNAManager::addDargLibForMedia(QList<MEDIAINFO> list)
{
    if(list.size()<=0)
        return;
    QList<dlnaLib> dlnaList;
    dlnaList.clear();
    for(int i=0;i<list.count();i++) {
        dlnaLib lib;
        lib.fileAddTime = LibData->getCurData();
        if(list.at(i).mediaType == 1)
            lib.fileCodec   = LibData->getVideoStream(list.at(i)).codec;
        else
            lib.fileCodec   = LibData->getMusicStream(list.at(i)).codec;
        lib.fileExt         = list.at(i).ext;
        lib.fileName        = list.at(i).title;
        lib.filePath        = QDir::fromNativeSeparators(list.at(i).filepath);
        lib.fileshot        = list.at(i).screenshot;
        lib.fileType        = list.at(i).mediaType;
        if(list.at(i).mediaType == 1) {
            lib.fileAsp         = LibData->getVideoStream(list.at(i)).aspectRatio;
        }

        lib.folderType      = 3;
        if(!isExitDlnaData(lib)) {
            _dlnaData.prepend(lib);
            dlnaList.append(lib);
        }
    }

    if(dlnaList.count()>0) {
        Sql->addDLnaListToSql(dlnaList);
        emit upShareWidget(true);
        emit upShareLeft(true);
    }
}

bool DLNAManager::getDLnaDetailVisible()
{
    bool isVisible = false;
    if(_selDlnaData.count() == 1) {
        if(_selDlnaData.at(0).folderType == 3) {
            if(libInfoSupper(_selDlnaData.at(0))) {
                isVisible = false;
            } else {
                isVisible = true;
            }
        }
    }
    return isVisible;
}

void DLNAManager::addDLnaListSlt(int count, QList<dlnaLib> dlnaList)
{
    if(dlnaList.size()<=0)
        return;
    for(int i=0;i<dlnaList.count();i++) {
        _dlnaData.insert(_addLibPos, dlnaList.at(i));
        _addLibPos++;
    }

    if(count == 0) {
        emit upShareWidget(true);
    } else {
        emit upShareWidget(false);
    }
    emit upAnalyzeInfo(dlnaList);
    emit upShareLeft(true);

}

void DLNAManager::sendGetDeviceSupSlt(deviceInfo dev)
{
    CONTROL->sendGetProtocolInfo(dev.udn);
}
