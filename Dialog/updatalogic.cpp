#include "updatalogic.h"
#include "globalarg.h"
#include "httprequest.h"
#include <QVersionNumber>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

#include <configure.h>
#include <QApplication>
#include "openfile.h"
UpdataLogic::UpdataLogic(QObject *parent) : QObject(parent)
{
    init();
}

void UpdataLogic::startCheckVersion(bool autoCheck)
{
    _autoCheck = autoCheck;
    if(_autoCheck) {
        if(_hasChecked)
            return;
    } else {
        _hasChecked = true;
    }

    _hasUseOld = false;
    if(_reply) {
        //防止请求未完成 重复多次
        _reply->deleteLater();
        _reply = NULL;
    }
    _versionData.clear();
    QString web = Global->getMainUpGradeWeb();
    //发出请求，获取文件信息;
    _reply = Http->get(web);
    //下面关联信号和槽
    connect(_reply,SIGNAL(finished()),this,SLOT(httpFinished()));
    //下载完成后
    connect(_reply,SIGNAL(readyRead()),this,SLOT(httpReadyRead()));
}

void UpdataLogic::init()
{
    _reply      = NULL;
    _autoCheck  = false;
    _hasChecked = false;
}

int UpdataLogic::checkUpVersion()
{
    int isUpgrade = 0;
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(_versionData, &error);
    if(error.error == QJsonParseError::NoError) {
        QJsonObject json_object = jsonDoc.object();
        QString lastVer    = json_object.value("version").toString();

        QStringList lastVsn= lastVer.split(".");
        QVersionNumber curVersion(Global->_version_maj, Global->_version_min);
        if(lastVsn.size() >= 2) {
            QVersionNumber lastVersion(lastVsn.at(0).toInt(),lastVsn.at(1).toInt());
            if(lastVersion > curVersion) {
                isUpgrade = 1;
            }
        }
    } else {
        isUpgrade = 2;
        qDebug("check Version json error");
    }
    return isUpgrade;
}

void UpdataLogic::startUserOldWebCheck()
{
    if(_hasUseOld) {
        emit upCheckFail();
        return;
    }
    _hasUseOld = true;
    if(_reply) {
        //防止请求未完成 重复多次
        _reply->deleteLater();
        _reply = NULL;
    }

    _versionData.clear();
    QString web = "http://www.5kplayer.com/upgrade/windows/upgrade5KPlayer.config";

    //发出请求，获取文件信息;
    _reply = Http->get(web);
    //下面关联信号和槽
    connect(_reply,SIGNAL(finished()),this,SLOT(httpFinished()));
    //下载完成后
    connect(_reply,SIGNAL(readyRead()),this,SLOT(httpReadyRead()));
}

void UpdataLogic::httpReadyRead()
{
    QVariant status_code = _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(status_code.toInt() != 200)
        return;
    _versionData = _reply->readAll();
}

void UpdataLogic::httpFinished()
{

    QVariant status_code = _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(status_code.toInt() != 200) {
        startUserOldWebCheck();
    } else {
        int checkFlag = checkUpVersion();
        if(checkFlag != 2) {
            if(checkFlag == 1) {
                if(_autoCheck) {
                    emit upAutoUP(_versionData);
                } else {
                    emit upNeedUp(_versionData);
                }
            } else {
                emit upNoUp();
            }
        Global->setCheckUpDate(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
        } else {
            emit upCheckFail();
        }
    }
    _reply->deleteLater();
    _reply = NULL;
}
