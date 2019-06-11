#include "upgradeytb.h"
#include "globalarg.h"
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QFileInfo>
#include <QApplication>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include "httprequest.h"

UpGradeYtb::UpGradeYtb(QObject *parent) : QObject(parent)
{
    _filePath   = Global->getuserYtbPaht().append("/youtube_dl.zip");
    _lastDate   = "2017-01-01";
    _isCheck    = false;
    qsrand(QDateTime::currentMSecsSinceEpoch());
}

void UpGradeYtb::startUpGrade()
{
    if(Global->_down_ytb)
        return;
    QFileInfo info(_filePath);
    if(info.fileName().isEmpty())
        return;
    if(info.exists())
        QFile::remove(_filePath);
    _file = new QFile(_filePath);
    if(!_file->open(QIODevice::WriteOnly | QIODevice::Append)){
        delete _file;
        _file = 0;
        return;
    }

    if(_downReply) {
        //防止请求未完成 重复多次
        _downReply->deleteLater();
        _downReply = NULL;
    }
    int index = qrand()% (_ytbDownWebList.count());
    QString fileWeb = _ytbDownWebList.at(index);

    _downReply = Http->get(fileWeb) ;
    if (!_downReply)
        return;

    Global->_down_ytb   = true;
    _useOldDown         = false;
    //下面关联信号和槽
    //下载完成后
    connect(_downReply,SIGNAL(finished()),this,SLOT(httpFinished()));
    //有可用数据时
    connect(_downReply,SIGNAL(readyRead()),this,SLOT(httpReadyRead()));
    connect(_downReply,SIGNAL(downloadProgress(qint64,qint64)),SLOT(ytbDownloadProgress(qint64,qint64)));
}

void UpGradeYtb::startUpGradeUserOldDown()
{
    if(_useOldDown) {
        emit ytbUpFail();
        return;
    }
    _useOldDown = true;
    QString web = "http://dl1.5kplayer.com/youtube/youtube_dl.zip";

    QFileInfo info(_filePath);
    if(info.fileName().isEmpty())
        return;
    if(info.exists())
        QFile::remove(_filePath);
    _file = new QFile(_filePath);
    if(!_file->open(QIODevice::WriteOnly | QIODevice::Append)){
        delete _file;
        _file = 0;
        return;
    }

    if(_downReply) {
        //防止请求未完成 重复多次
        _downReply->deleteLater();
        _downReply = NULL;
    }

    _downReply = Http->get(web) ;
    if (!_downReply)
        return;

    Global->_down_ytb = true;
    //下面关联信号和槽
    //下载完成后
    connect(_downReply,SIGNAL(finished()),this,SLOT(httpFinished()));
    //有可用数据时
    connect(_downReply,SIGNAL(readyRead()),this,SLOT(httpReadyRead()));
    connect(_downReply,SIGNAL(downloadProgress(qint64,qint64)),SLOT(ytbDownloadProgress(qint64,qint64)));
}

void UpGradeYtb::checkUpGrade(bool ischeck)
{
    _isCheck = ischeck;
    _useOldWeb = false;
    QString web = Global->getYtbUpGradeWeb();
    _ytbData.clear();
    if(_handReply) {
        //防止请求未完成 重复多次
        _handReply->disconnect();
        _handReply->abort();
        _handReply->deleteLater();
        _handReply = NULL;
    }

    _handReply = Http->get(web);
    if (!_handReply)
        return;

    connect(_handReply,SIGNAL(readyRead()),SLOT(httpHandReadyRead()));
    connect(_handReply, SIGNAL(finished()),SLOT(httpHandFinished()));
}

void UpGradeYtb::ytbExtract()
{
    QString toPath = Global->getuserYtbPaht().append("/");
    if(Global->Extract(_filePath,toPath)) {//解压缩,会自动覆盖文件
        QFile::remove(_filePath);
        Global->setYtbdownFinish(false);
        Global->setYtbLastModified(_lastDate);
        Global->setYtbLastCheckDate(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    }
}

void UpGradeYtb::checkUpGradeUserOldWeb()
{
    if(_useOldWeb) {
        emit ytbUpFail();
        return;
    }
    _useOldWeb = true;
    QString web = "http://dl1.5kplayer.com/youtube/upgradeYoutube.config";
    _ytbData.clear();
    if(_handReply) {
        //防止请求未完成 重复多次
        _handReply->deleteLater();
        _handReply = NULL;
    }

    _handReply = Http->get(web);
    if (!_handReply)
        return;

    connect(_handReply,SIGNAL(readyRead()),SLOT(httpHandReadyRead()));
    connect(_handReply, SIGNAL(finished()),SLOT(httpHandFinished()));
}

void UpGradeYtb::httpReadyRead()
{
    QVariant status_code = _downReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(status_code.toInt() != 200)
        return;
    if (_file) {
        _file->write(_downReply->readAll());  //如果文件存在，则写入文件
    }
}

void UpGradeYtb::httpFinished()
{
    _file->flush();
    _file->close();
    delete _file;
    _file = 0;
    QVariant status_code = _downReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(status_code.toInt() == 200) {
        _downReply->deleteLater();
        _downReply = NULL;
        Global->_down_ytb = false;
        QString toPath = Global->getuserYtbPaht().append("/");
        if(QFileInfo::exists(_filePath)) {
            // ytb模块在使用时，先不解压，向ini文件中添加一个下载完成但未解压的标记
            if(Global->_user_ytb) {
                Global->setYtbdownFinish(true);
            } else {
                if(Global->Extract(_filePath,toPath)) {//解压缩,会自动覆盖文件
                    QFile::remove(_filePath);
                    Global->setYtbLastModified(_lastDate);
                    Global->setYtbLastCheckDate(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
                    emit ytbUpFinish();
                }
            }
        }
    } else {
        startUpGradeUserOldDown();
    }
}

void UpGradeYtb::ytdDownCancel()
{
    Global->_down_ytb = false;
    if(_downReply) {
        _downReply->deleteLater();
        _downReply = NULL;
    }
    if(_file) {
        _file->flush();
        _file->close();
        delete _file;
        _file = 0;
    }
}

void UpGradeYtb::httpHandReadyRead()
{
    QVariant status_code = _handReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(status_code.toInt() != 200)
        return;
    _ytbData = _handReply->readAll();
}

void UpGradeYtb::httpHandFinished()
{
    QVariant status_code = _handReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(status_code.toInt() != 200) {
        checkUpGradeUserOldWeb();
    } else {
        QJsonParseError error;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(_ytbData, &error);
        if(error.error == QJsonParseError::NoError) {
            QJsonObject json_object = jsonDoc.object();
            _lastDate = json_object.value("lastModif").toString();
            QString upcheckUrl= json_object.value("upcheckUrl").toString().trimmed();
            if(!upcheckUrl.isEmpty())
                Global->setYtbUpGradeWeb(upcheckUrl);
            QJsonObject json_url = json_object.value("upgradeUrl").toObject();
            _ytbDownWebList.clear();
            foreach (QString key, json_url.keys()) {
                _ytbDownWebList.append(json_url.value(key).toString());
            }
            QString Updata = Global->getYtbLastModified();
            const QDateTime curDTime    = QDateTime::fromString(Updata,"yyyy-MM-dd");
            const QDateTime lastDTime   = QDateTime::fromString(_lastDate, "yyyy-MM-dd");

            qint64 day = curDTime.daysTo(lastDTime);
            if(day > 0 || _isCheck) {
                startUpGrade();
            } else {
                Global->setYtbLastCheckDate(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
            }
        } else {
            qDebug("json 数据错误");
        }
    }
}

void UpGradeYtb::ytbDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    QVariant status_code = _downReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(status_code.toInt() != 200)
        return;
    int progress = ((double)bytesReceived/bytesTotal)*100;
    emit ytbUpProgress(progress);
}
