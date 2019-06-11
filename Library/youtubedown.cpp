#include "youtubedown.h"
#include "librarydata.h"
#include <QApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QPixmap>
#include <QJsonDocument>
#include <QBuffer>
#include <QtDebug>
#include <QTextCodec>
#include "sqloperation.h"
#include "globalarg.h"
#include <QFileInfo>
#include "openfile.h"
#include "YtbTool.h"

QList<DOWNURL> YoutubeDown::getDownList()
{
    return _downList;
}

QList<URLMEDIADATA> YoutubeDown::getSubTitleList()
{
    return _subtitleData;
}

QList<MEDIAINFO> YoutubeDown::getSubAnalyzeList()
{
    return _analySubList;
}

void YoutubeDown::upSubTitleList(const MEDIAINFO &media, const QStringList &subtitle)
{
    bool addflag = true;
    int  Pos = 0;
    for(int i=0;i<_subtitleData.size();++i) {
        if(_subtitleData.at(i).mediainfo.webUrl == media.webUrl) {
            Pos = i;
            addflag = false;
            break;
        }
    }
    if(addflag) {
        URLMEDIADATA subdata;
        subdata.mediainfo       = media;
        subdata.checkSubtitle   = subtitle;
        _subtitleData.append(subdata);
    } else {
        URLMEDIADATA subdata;
        subdata.mediainfo       = media;
        subdata.checkSubtitle   = subtitle;
        _subtitleData.replace(Pos,subdata);
    }
}

void YoutubeDown::deletemedia(const MEDIAINFO &media)
{
    for(int i=0;i<_analyMediaList.size();i++) {
        if(_analyMediaList.at(i) == media.webUrl) {
            _ytbTool->stopThread(media.webUrl,AnalySource);
            _analyMediaList.removeAt(i);
            break;
        }
    }
    for(int i=0;i<_downList.size();++i) {
        if(_downList.at(i).downmedia.webUrl == media.webUrl) {
            _ytbTool->stopThread(media.webUrl,DownloadSource);
            _downList.removeAt(i);
            break;
        }
    }
    for(int i=0;i<_analySubList.size();++i) {
        if(_analySubList.at(i).webUrl == media.webUrl) {
            _ytbTool->stopThread(media.webUrl,AnalySubtitle);
            _analySubList.removeAt(i);
            break;
        }
    }
    for(int i=0;i<_subtitleData.size();++i) {
        if(_subtitleData.at(i).mediainfo.webUrl == media.webUrl) {
            _subtitleData.removeAt(i);
            break;
        }
    }
}

void YoutubeDown::clickDownbtn(const MEDIAINFO &media)
{
    if(!LibData->checkYtbDeploy()) {
        int res = OpenFileDialog->ytbDeploy();
        if(res == 0)
            emit sendDownProcess(media.webUrl,200);
            proStop(media);
            return;
    }
    // 下载列表，用于控制界面显示
    DOWNURL downUrl;
    downUrl.downmedia = media;
    if(!media.formatID.contains("+")) {
        downUrl.audioFlag = true;
    }
    _downList.append(downUrl);

    QStringList subList;
    subList.clear();
    for(int i=0;i<_subtitleData.size();i++) {
        if(_subtitleData.at(i).mediainfo.webUrl == media.webUrl) {
            subList = _subtitleData.at(i).checkSubtitle;
            break;
        }
    }

    DownloadArgs downArgs;
    downArgs.downInfo   = media;
    downArgs.sub        = subList;
    downArgs.ffmpegPath = Global->getuserYtbPaht();
    downArgs.outputPath = Global->downloadPath();
    downArgs.appPath    = _ytbFilepath;
    if(Global->openProxy()) {
        downArgs.https = Global->proxyUrl();
    }
}

void YoutubeDown::proStop(const MEDIAINFO &media)
{
    for(int i=0;i<_downList.size();++i) {
        if(_downList.at(i).downmedia.webUrl == media.webUrl) {
            _ytbTool->stopThread(media.webUrl,DownloadSource);
            _downList.removeAt(i);
            break;
        }
    }
}

void YoutubeDown::clickAnalyzeSub(const MEDIAINFO &media)
{
    _analySubList.append(media);

    AnalysisArgs analyArgs;
    analyArgs.url   = media.webUrl;
    if(Global->openProxy()) {
        analyArgs.https = Global->proxyUrl();
    }
    analyArgs.appPath = _ytbFilepath;
    _ytbTool->analysisSub(analyArgs);
}

void YoutubeDown::analypathFinish(QString url, QList<MEDIAINFO> mediaList)
{
    for(int i=0;_analyMediaList.size();i++) {
        if(_analyMediaList.at(i) == url) {
            _analyMediaList.removeAt(i);
            break;
        }
    }

    if(mediaList.size() > 0) {
        emit analyFinsh(url,mediaList);
    } else {
        emit analyError(url);
    }
}

void YoutubeDown::analySubFinish(const QString &url, const QStringList &subList)
{
    for(int i=0;i<_analySubList.size();i++) {
        if(_analySubList.at(i).webUrl == url) {
            MEDIAINFO media;
            media = _analySubList.at(i);
            media.subtitleList = subList;
            emit sendanalyzeinfo(media);
            _analySubList.removeAt(i);
            break;
        }
    }
}

void YoutubeDown::downProcessSlot(const QString &url, const double &process)
{
    for(int i=0;i<_downList.size();i++) {
        if(_downList.at(i).downmedia.webUrl == url) {
            DOWNURL downinfo;
            downinfo.downmedia      = _downList.at(i).downmedia;
            downinfo.audioFlag      = _downList.at(i).audioFlag;
            downinfo.downProcess    = process;
            _downList.replace(i,downinfo);
            break;
        }
    }
    emit sendDownProcess(url,process);
}

void YoutubeDown::downFinish(const QString &url, const QString &fileName)
{
    for(int i=0;i<_downList.size();i++){
        DOWNURL urlMedia = _downList.at(i);
        if(urlMedia.downmedia.webUrl == url) {
            if(urlMedia.audioFlag) {
                addtoMediaList(fileName,"ytb");
            } else {
                addtoMediaList(fileName,"yVideos");
            }
            emit removeUrlWidget(urlMedia.downmedia);
            _downList.removeAt(i);
            break;
        }
    }
}

void YoutubeDown::downFail(const QString &url)
{
    for(int i=0;i<_downList.size();i++) {
        if(_downList.at(i).downmedia.webUrl == url) {
            _downList.removeAt(i);
            break;
        }
    }
    emit sendDownProcess(url,200);
}

YoutubeDown::YoutubeDown(QObject *parent)
    : QObject(parent)
{
    _ytbFilepath = Global->getuserYtbPaht().append("/ytb.exe");
    _ytbTool     = new YtbTool(this);

    _downList.clear();
    _analyMediaList.clear();
    _analySubList.clear();
    _subtitleData.clear();

    connect(_ytbTool,SIGNAL(analysisFinish(QString,QList<MEDIAINFO>)),SLOT(analypathFinish(QString,QList<MEDIAINFO>)));
    connect(_ytbTool,SIGNAL(analysisSubFinish(QString,QStringList)),SLOT(analySubFinish(QString,QStringList)));
    connect(_ytbTool,SIGNAL(downprogress(QString,double)),SLOT(downProcessSlot(QString,double)));
    connect(_ytbTool,SIGNAL(downFinish(QString,QString)),SLOT(downFinish(QString,QString)));
    connect(_ytbTool,SIGNAL(downFail(QString)),SLOT(downFail(QString)));
}

void YoutubeDown::addtoMediaList(QString path, QString item)
{
    LibData->addmedia(path,item,true);
}

QString YoutubeDown::getWinFileName(QString title)
{
    QString winStr = "\\/:*?\"<>|";
    for(int i=title.length()-1;i>=0;--i) {
        for(int j= winStr.length()-1;j>=0;--j) {
            if(title.at(i) == winStr.at(j)) {
                title.remove(i,1);
                break;
            }
        }
    }
    return title;
}

QStringList YoutubeDown::getSelectSubList(QString url)
{
    QStringList subList;
    subList.clear();
    for(int i=0;i<_subtitleData.size();++i) {
        if(_subtitleData.at(i).mediainfo.webUrl == url){
            subList = _subtitleData.at(i).checkSubtitle;
            break;
        }
    }
    return subList;
}

DOWNURL YoutubeDown::getDownMedia(QString url)
{
    DOWNURL downMedia;
    for(int i=0;i<_downList.size();++i) {
        if(_downList.at(i).downmedia.webUrl == url) {
            downMedia = _downList.at(i);
            break;
        }
    }
    return downMedia;
}

QStringList YoutubeDown::analyzeSubtitles(QByteArray subByte)
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
//                    substu.language = language.at(0);
//                    substu.languageformat = language.at(1);
//                    subtitleList.append(substu);
                }
            }
        }
    }

    return subtitleList;
}

void YoutubeDown::startAnalyPath(QString path)
{
    _analyMediaList.append(path);
    AnalysisArgs analyArgs;
    analyArgs.url   = path;
    if(Global->openProxy()) {
        analyArgs.https = Global->proxyUrl();
    }
    analyArgs.appPath = _ytbFilepath;
    _ytbTool->analysisUrl(analyArgs);
}

_StreamInfo YoutubeDown::getformatStream(const MEDIAINFO media)
{
    QString formats = media.formatID;
    QString selectStream;
    _StreamInfo stream;
    if(formats.contains("+")) {
        QStringList formatIds = formats.split("+");
        selectStream = formatIds.at(0);
    } else {
        selectStream = formats;
    }
    for(int i=0;i<media.streamList.size();i++) {
        if(media.streamList.at(i).streamId == selectStream) {
            stream = media.streamList.at(i);
            break;
        }
    }
    return stream;
}
