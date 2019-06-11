#include "adlogic.h"
#include "globalarg.h"
#include "httprequest.h"
#include "ggwidget.h"

#include <QFile>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include <QDir>
#include <QFileInfoList>
#include <QTimer>
#include "applenotify.h"
#include "librarydata.h"

#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

//一天的毫秒数
#define ONEDAY_MSEC 1*24*60*60*1000

ADLogic::ADLogic(QObject *parent) : QObject(parent)
{
    _adFolderpath   = Global->adFloderPath();
    _adLanguage     = Global->getLanguage();
    qsrand(QDateTime::currentMSecsSinceEpoch());
    _autoPushTimer      = new QTimer(this);
    _adConfigTimer      = new QTimer(this);
    _adConfigTimer->setInterval(3600*24*1000);
    _adConfigTimer->start();
    _ggWidget       = new GGWidget;
    _adApple        = new AppleNotify;

    initAdini();
    getLocalAdFile();
    QDateTime configUp      = QDateTime::fromString(Global->getAdsConfigTime(),"yyyy-MM-dd hh-mm-ss");
    if(configUp.daysTo(QDateTime::currentDateTime()) > 1 || configUp.daysTo(QDateTime::currentDateTime()) < 0 || _localFile.adfileList.size() < 6) {
        getAdFileByWeb();
    }

    //getLocalArea();
    //_localPreference = getPreferInfoence();

    _adApple->supportIPhone();
    _adApple->supportIPad();
    connect(_adApple,SIGNAL(appleDeviceInserted()),SLOT(appleNotifySlt()));
    connect(_autoPushTimer,SIGNAL(timeout()),SLOT(autoPushTimerOut()));
    connect(_adConfigTimer,SIGNAL(timeout()),SLOT(adUpConfigOut()));
    connect(_ggWidget,SIGNAL(clickWinKey(QString)),SLOT(openUrlSlt(QString)));
    connect(LibData,SIGNAL(upplaymedia(MEDIAINFO)),SLOT(playMediaChange(MEDIAINFO)));
}

void ADLogic::startAdByAuto()
{
    // 检查版本信息 响应升级广告推送
    int showTime = 300 + (qrand()% 300);
    _autoPushTimer->start(showTime*1000);
}

void ADLogic::getLocalArea()
{
    QString area = Global->getLocalArea();
    QString areaDate = Global->getLocalAreaTime();
    const QDateTime curDTime    = QDateTime::currentDateTime();
    const QDateTime lastDTime   = QDateTime::fromString(areaDate, "yyyy-MM-dd");

    qint64 day = lastDTime.daysTo(curDTime);
    bool checkArea = false;
    if(day > 7 || day < 0) {
        checkArea = true;
    }

    if(area.isEmpty() || checkArea) {
        _adArea.clear();
        if(_areaReply) {
            //防止请求未完成 重复多次
            _areaReply->disconnect();
            _areaReply->abort();
            _areaReply->deleteLater();
            _areaReply = NULL;
        }

        _areaReply = Http->get("http://www.5kplayer.com/common/cc.php");
        if (!_areaReply)
            return;

        connect(_areaReply,SIGNAL(readyRead()),SLOT(httpAreaRead()));
        connect(_areaReply, SIGNAL(finished()),SLOT(httpAreaFinish()));
    }
}

void ADLogic::getLocalAdFile()
{
    adWebFile clearFile;
    _localFile = clearFile;
    QString adPath = _adFolderpath + "/" + Global->getLanguage() + "/ad.config";
    QFile file(adPath);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray jsonByte = QByteArray::fromBase64(file.readAll());
        QJsonParseError error;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonByte, &error);
        if(error.error == QJsonParseError::NoError) {
            QJsonObject json_object = jsonDoc.object();
            _localFile.interval = json_object.value("interval").toInt();
            _localFile.noadFlag = json_object.value("no_showad").toBool();

            QString nexts = json_object.value("next").toString();
            QByteArray byte = decodeAes(randomKey,QByteArray::fromHex(nexts.toUtf8()));
            _localFile.nextUrl  = QString::fromUtf8(byte);
            qDebug() << "webInfo.nextUrl:" << _localFile.nextUrl;

            QJsonArray jsonArray = json_object.value("zip").toArray();
            for(int i=0;i<jsonArray.size();++i) {
                adFile adInfo;
                adInfo.url          = jsonArray.at(i).toObject().value("info").toString();
                adInfo.type         = jsonArray.at(i).toObject().value("type").toString();
                adInfo.number       = jsonArray.at(i).toObject().value("number").toInt();
                adInfo.prob         = jsonArray.at(i).toObject().value("prob").toInt();
                adInfo.language     = jsonArray.at(i).toObject().value("language").toString();
                QString areaList = jsonArray.at(i).toObject().value("area").toString();
                adInfo.area = areaList.split(",");
                if(checkLocalFileIsExit(adInfo.number))
                    _localFile.adfileList.append(adInfo);
            }
        }
    }
}

bool ADLogic::checkLocalFileIsExit(int fileNumber)
{
    bool isExit = false;
    QString adText = QString::number(fileNumber);
    QString loadFolder = Global->adFloderPath() + "/" + Global->getLanguage() + "/" + adText;
    QString loadFile = loadFolder + "/" + adText + ".json";
    QDir dir(loadFolder);
    if(dir.exists()) {
        QFile file(loadFile);
        if(file.exists()) {
            if(file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QByteArray jsonByte = QByteArray::fromBase64(file.readAll());
                QJsonParseError error;
                QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonByte, &error);
                if(error.error == QJsonParseError::NoError) {
                    QJsonObject json_object = jsonDoc.object();
                    int width = json_object.value("ad_height").toInt();
                    int height= json_object.value("ad_width").toInt();
                    if(width > 0 && height > 0)
                        isExit = true;
                }
            }
        }
    }
    return isExit;
}

void ADLogic::getAdFileByWeb()
{
    _adData.clear();
    _iswebFinish    = false;
    if(_handReply) {
        //防止请求未完成 重复多次
        _handReply->disconnect();
        _handReply->abort();
        _handReply->deleteLater();
        _handReply = NULL;
    }

    // 广告获取地址

    QString adUrl = "http://dl1.5kplayer.com/5kpads/ad.config";
    if(!_localFile.nextUrl.isEmpty()) {
        _handReply = Http->get(_localFile.nextUrl);
    } else {
        _useFixedWeb = true;
        _handReply = Http->get(adUrl);
    }

    if (!_handReply)
        return;

    connect(_handReply,SIGNAL(readyRead()),SLOT(httpWebReadyRead()));
    connect(_handReply, SIGNAL(finished()),SLOT(httpWebFinished()));
}

void ADLogic::getAdFileByFixedWeb()
{
    if(_useFixedWeb) {
        return;
    }

    _adData.clear();
    _iswebFinish    = false;
    if(_handReply) {
        //防止请求未完成 重复多次
        _handReply->disconnect();
        _handReply->abort();
        _handReply->deleteLater();
        _handReply = NULL;
    }

    // 广告获取地址
    QString adUrl = "http://dl1.5kplayer.com/5kpads/ad.config";
    _handReply      = Http->get(adUrl);
    _useFixedWeb    = true;

    if (!_handReply)
        return;

    connect(_handReply,SIGNAL(readyRead()),SLOT(httpWebReadyRead()));
    connect(_handReply, SIGNAL(finished()),SLOT(httpWebFinished()));
}

bool ADLogic::getLocalAdini()
{
    bool isOk = false;
    QString adPath = _adFolderpath + "/ads.ini";
    QFile file(adPath);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray jsonByte = QByteArray::fromBase64(file.readAll());
        readWebAdini(jsonByte);
        isOk = true;
    }
    return isOk;
}

void ADLogic::getWebAdini()
{
    _adsConfigData.clear();
    _useOldiniWeb = false;
    if(_adsConfigReply) {
        //防止请求未完成 重复多次
        _adsConfigReply->disconnect();
        _adsConfigReply->abort();
        _adsConfigReply->deleteLater();
        _adsConfigReply = NULL;
    }

    // 广告获取地址

    QString adUrl = "http://dl1.5kplayer.com/5kpads/ads.ini";
    if(!_adsConInfo.nextUrl.isEmpty()) {
        adUrl = _adsConInfo.nextUrl;
    }


    _adsConfigReply = Http->get(adUrl);

    if (!_adsConfigReply)
        return;

    connect(_adsConfigReply,SIGNAL(readyRead()),SLOT(httpAdsReadyRead()));
    connect(_adsConfigReply, SIGNAL(finished()),SLOT(httpAdsFinished()));
}

void ADLogic::getOldWebAdini()
{
    if(_useOldiniWeb)
        return;
    _useOldiniWeb = true;
    _adsConfigData.clear();
    if(_adsConfigReply) {
        //防止请求未完成 重复多次
        _adsConfigReply->disconnect();
        _adsConfigReply->abort();
        _adsConfigReply->deleteLater();
        _adsConfigReply = NULL;
    }

    // 广告获取地址

    QString adUrl = "http://dl1.5kplayer.com/5kpads/ads.ini";

    _adsConfigReply = Http->get(adUrl);

    if (!_adsConfigReply)
        return;

    connect(_adsConfigReply,SIGNAL(readyRead()),SLOT(httpAdsReadyRead()));
    connect(_adsConfigReply, SIGNAL(finished()),SLOT(httpAdsFinished()));
}

void ADLogic::writeLocalAdini()
{
    QJsonObject dvdObj;// dvd
    dvdObj.insert("dvd_interval", _adsConInfo.dvdConfig.interval);

    QJsonObject iPhoneObj;// iPhone
    iPhoneObj.insert("iPhone_interval", _adsConInfo.iPhoneConfig.interval);

    QJsonObject convertObj;// convert
    convertObj.insert("convert_interval",_adsConInfo.convertConfig.interval);
    convertObj.insert("convert_size",_adsConInfo.convertConfig.fileSize);
    convertObj.insert("convert_res_w",_adsConInfo.convertConfig.resolution.width());
    convertObj.insert("convert_res_h",_adsConInfo.convertConfig.resolution.height());
    QJsonArray conListObj;
    for(int i=0;i<_adsConInfo.convertConfig.conMediaList.size();i++) {
        QJsonObject comObj;
        comObj.insert("convert_media_fmt",_adsConInfo.convertConfig.conMediaList.at(i).mediaFormat);
        comObj.insert("convert_video_fmt",_adsConInfo.convertConfig.conMediaList.at(i).videoFormat);
        comObj.insert("convert_audio_fmt",_adsConInfo.convertConfig.conMediaList.at(i).audioFormat);
        conListObj.append(comObj);
    }
    convertObj.insert("convert_fmt",conListObj);

    QJsonObject json;
    json.insert("next",         _adsConInfo.nextUrl);
    json.insert("dvd",          dvdObj);
    json.insert("iPhone",       iPhoneObj);
    json.insert("convert",      convertObj);

    QJsonDocument document;
    document.setObject(json);
    QByteArray byte_array = document.toJson(QJsonDocument::Indented);

    QString iniPath = Global->adFloderPath() + "/ads.ini";
    QFile fi2(iniPath);
    if(fi2.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        fi2.write(byte_array.toBase64());
        fi2.flush();
        fi2.close();
    }
}

void ADLogic::readWebAdini(QByteArray data)
{
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &error);
    if(error.error == QJsonParseError::NoError) {
        QJsonObject json_object = jsonDoc.object();
        _adsConInfo.nextUrl     = json_object.value("next").toString();
        QJsonObject dvdObj      = json_object.value("dvd").toObject();
        _adsConInfo.dvdConfig.interval = dvdObj.value("dvd_interval").toInt();

        QJsonObject iPhoneObj   = json_object.value("iPhone").toObject();
        _adsConInfo.iPhoneConfig.interval = iPhoneObj.value("iPhone_interval").toInt();

        QJsonObject convertObj  = json_object.value("convert").toObject();
        _adsConInfo.convertConfig.interval = convertObj.value("convert_interval").toInt();
        _adsConInfo.convertConfig.fileSize = convertObj.value("convert_size").toDouble();
        _adsConInfo.convertConfig.resolution.setHeight(convertObj.value("convert_res_h").toInt());
        _adsConInfo.convertConfig.resolution.setWidth(convertObj.value("convert_res_w").toInt());
        QJsonArray conListObj = convertObj.value("convert_fmt").toArray();
        if(conListObj.size() > 0)
            _adsConInfo.convertConfig.conMediaList.clear();
        for(int i=0;i<conListObj.size();i++) {
            convertFormat confmt;
            confmt.mediaFormat = conListObj.at(i).toObject().value("convert_media_fmt").toString();
            confmt.videoFormat = conListObj.at(i).toObject().value("convert_video_fmt").toString();
            confmt.audioFormat = conListObj.at(i).toObject().value("convert_audio_fmt").toString();
            _adsConInfo.convertConfig.conMediaList.append(confmt);
        }
    }
}

QString ADLogic::getAdWinKey(int number)
{
    // 加载文件
    QString winKye = "";
    QString adText = QString::number(number);
    QString loadFolder = Global->adFloderPath() + "/" + Global->getLanguage() + "/"+ adText;
    QString loadFile = loadFolder + "/" + adText + ".json";
    QFile file(loadFile);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray jsonByte = QByteArray::fromBase64(file.readAll());
        QJsonParseError error;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonByte, &error);
        if(error.error == QJsonParseError::NoError) {
            QJsonObject json_object = jsonDoc.object();
            winKye = json_object.value("ad_winkey").toString();
        }
    }

    return winKye;
}

adWebFile ADLogic::analyJsonByByteArray(QByteArray bytes)
{
    adWebFile webInfo;
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(QByteArray::fromBase64(bytes), &error);
    if(error.error == QJsonParseError::NoError) {
        QJsonObject json_object = jsonDoc.object();

        webInfo.interval = json_object.value("interval").toInt();
        webInfo.noadFlag = json_object.value("no_showad").toBool();
        QString nexts = json_object.value("next").toString();
        int    google = json_object.value("google").toInt();
        Global->setGoogleRate(google);

        QByteArray byte = decodeAes(randomKey,QByteArray::fromHex(nexts.toUtf8()));
        webInfo.nextUrl  = QString::fromUtf8(byte);
        qDebug() << "analyJsonByByteArray webInfo.nextUrl:" << webInfo.nextUrl;

        QJsonArray jsonArray = json_object.value("zip").toArray();
        for(int i=0;i<jsonArray.size();++i) {
            adFile adInfo;
            adInfo.url      = jsonArray.at(i).toObject().value("url").toString();
            adInfo.type     = jsonArray.at(i).toObject().value("type").toString();
            adInfo.number   = jsonArray.at(i).toObject().value("number").toInt();
            adInfo.prob     = jsonArray.at(i).toObject().value("prob").toInt();
            QString areaList = jsonArray.at(i).toObject().value("area").toString();
            adInfo.language = jsonArray.at(i).toObject().value("language").toString();
            adInfo.area = areaList.split(",");
//            if(adInfo.area.contains(Global->getLocalArea()) || areaList.isEmpty())
            if(_adLanguage == adInfo.language)
                webInfo.adfileList.append(adInfo);
        }
    }
    return webInfo;
}

void ADLogic::writeLocalAdFile()
{
    QJsonObject json;
    json.insert("interval", _localFile.interval);
    json.insert("no_showad",_localFile.noadFlag);

    QByteArray nextByte = encodeAes(randomKey,_localFile.nextUrl.toUtf8());
    json.insert("next",         QString(nextByte.toHex()));

    QJsonArray jsonArray;
    for(int i=0;i<_localFile.adfileList.size();i++) {
        QJsonObject adFileJson;
        adFileJson.insert("info",   _localFile.adfileList.at(i).url);
        adFileJson.insert("type",   _localFile.adfileList.at(i).type);
        adFileJson.insert("number", _localFile.adfileList.at(i).number);
        adFileJson.insert("language", _localFile.adfileList.at(i).language);
        adFileJson.insert("prob",   _localFile.adfileList.at(i).prob);
        QString area = "";
        for(int j=0;j<_localFile.adfileList.at(i).area.size();j++) {
            QString val = _localFile.adfileList.at(i).area.at(j);
            if(j == _localFile.adfileList.at(i).area.size() - 1) {
                area.append(val);
            } else {
                area.append(val).append(",");
            }
        }
        adFileJson.insert("area",area);
        jsonArray.append(adFileJson);
    }

    json.insert("zip",jsonArray);

    QJsonDocument document;
    document.setObject(json);
    QByteArray byte_array = document.toJson(QJsonDocument::Indented);

    QString adPath = _adFolderpath + "/" + Global->getLanguage() + "/ad.config";
    QFile fi(adPath);
    if(fi.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        fi.write(byte_array.toBase64());
        fi.flush();
        fi.close();
    }
}

QList<adFile> ADLogic::comparedLocalOrWeb()
{
    QList<adFile> downList;
    QList<adFile> deleteList;
    downList.clear();
    deleteList.clear();

    _localFile.interval     = _webFile.interval;
    _localFile.nextUrl      = _webFile.nextUrl;
    _localFile.noadFlag     = _webFile.noadFlag;

    for(int i=0;i<_webFile.adfileList.size();i++) {
        bool isExit = true;
        for(int j=0;j<_localFile.adfileList.size();j++) {
            if(_webFile.adfileList.at(i).number == _localFile.adfileList.at(j).number) {
                isExit = false;
                break;
            }
        }
        if(isExit)
            downList.append(_webFile.adfileList.at(i));
    }
    for(int i=0;i<_localFile.adfileList.size();i++) {
        bool isExit = true;
        for(int j=0;j<_webFile.adfileList.size();j++) {
            if(_webFile.adfileList.at(j).number == _localFile.adfileList.at(i).number) {
                isExit = false;
                break;
            }
        }
        if(isExit)
            deleteList.append(_localFile.adfileList.at(i));
    }

    if(deleteList.size() > 0) {
        for(int i=0;i<deleteList.size();i++) {
            QString delPath = _adFolderpath + "/" + Global->getLanguage() + "/" + QString::number(deleteList.at(i).number);
            // 删除成功
            deleteFolder(delPath);
            for(int j = _localFile.adfileList.size() - 1;j>=0;j--) {
                if(_localFile.adfileList.at(j).number == deleteList.at(i).number) {
                    _localFile.adfileList.removeAt(j);
                    break;
                }
            }
        }
    }

    // 写入删除后的json文件到本地
    writeLocalAdFile();
    return downList;
}

bool ADLogic::deleteFolder(QString path)
{
    if (path.isEmpty()){
        return false;
    }
    QDir dir(path);
    if(!dir.exists()){
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
    foreach (QFileInfo file, fileList){ //遍历文件信息
        if (file.isFile()){ // 是文件，删除
            file.dir().remove(file.fileName());
        }else{ // 递归删除
            deleteFolder(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath()); // 删除文件夹
}

bool ADLogic::checkAdvertData(int number)
{
    // 加载文件
    bool checkAdvert = false;
    QString adText = QString::number(number);
    QString loadFolder = Global->adFloderPath() + "/" +Global->getLanguage() + "/"+ adText;
    QString loadFile = loadFolder + "/" + adText + ".json";
    QFile file(loadFile);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray jsonByte = QByteArray::fromBase64(file.readAll());
        QJsonParseError error;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonByte, &error);
        if(error.error == QJsonParseError::NoError) {
            QJsonObject json_object = jsonDoc.object();
            QString adStartDate = json_object.value("ad_start_date").toString();
            QString adEndDate   = json_object.value("ad_end_date").toString();
            QDateTime curDate   = QDateTime::currentDateTime();
            QDateTime startDate = QDateTime::fromString(adStartDate,"yyyy-M-d");
            QDateTime endDate   = QDateTime::fromString(adEndDate,"yyyy-M-d");
            if(startDate <= curDate && curDate <= endDate) {
                checkAdvert = true;
            }
        }
    }
    return checkAdvert;
}

void ADLogic::startDownWebFile(const QString &url)
{
    _downUrl = url;
    QFileInfo info(_downUrl);
    _downFilePath = Global->adFloderTemp() + "/" + info.completeBaseName() + ".Zip";
    _downFile = new QFile(_downFilePath);
    if(!_downFile->open(QIODevice::WriteOnly | QIODevice::Truncate)){
        delete _downFile;
        _downFile = 0;
        emit downFail(_downUrl);
        return;
    }
    //防止请求未完成 重复多次
    if(_downReply) {
        _downReply->deleteLater();
        _downReply = NULL;
    }

    _downReply = Http->get(_downUrl) ;
    if (!_downReply) {
        emit downFail(_downUrl);
        return;
    }

    connect(_downReply,SIGNAL(finished()),this,SLOT(webDownFinish()));
    connect(_downReply,SIGNAL(readyRead()),this,SLOT(webDownRead()));
}

void ADLogic::downFinish(const QString &url)
{
    for(int i=0;i<_downAdList.size();i++) {
        if(_downAdList.at(i).url == url) {
            _localFile.adfileList.append(_downAdList.at(i));
            writeLocalAdFile();
            _downAdList.removeAt(i);
            break;
        }
    }
    if(_downAdList.size() > 0) {
        startDownWebFile(_downAdList.at(0).url);
    }
}

void ADLogic::downFail(const QString &url)
{
    // 清除下载缓存文件
    for(int i=0;i<_downAdList.size();i++) {
        if(_downAdList.at(i).url == url) {
            _downAdList.removeAt(i);
            break;
        }
    }
    if(_downAdList.size() > 0) {
        startDownWebFile(_downAdList.at(0).url);
    }
}

bool ADLogic::comparedSize(QSize tarSize, QSize souSize)
{
    bool iscom = false;
    if(tarSize.width() < tarSize.height()) { // 是长视频
        if(tarSize.height() > souSize.width() || tarSize.width() > souSize.height()) {
            iscom = true;
        }
    } else {
        if(tarSize.height() > souSize.height() || tarSize.width() > souSize.width()) {
            iscom = true;
        }
    }

    return iscom;
}

bool ADLogic::compareFormat(MEDIAINFO media)
{
    bool isformat = false;
    for(int i=0;i<_adsConInfo.convertConfig.conMediaList.size();i++) {
        if(QString::compare(media.ext,_adsConInfo.convertConfig.conMediaList.at(i).mediaFormat,Qt::CaseInsensitive) == 0
            && QString::compare(LibData->getVideoStream(media).codec,_adsConInfo.convertConfig.conMediaList.at(i).videoFormat,Qt::CaseInsensitive) == 0
            && QString::compare(LibData->getMusicStream(media).codec,_adsConInfo.convertConfig.conMediaList.at(i).audioFormat,Qt::CaseInsensitive) == 0) {
            isformat = true;
            break;
        }
    }
    return isformat;
}

bool ADLogic::isShowTimeOut(adFile fileInfo)
{
    QDateTime preShowTime;
    QDateTime curTime       = QDateTime::currentDateTime();

    bool show = false;
    // Ordinary类型的广告，
    if(fileInfo.type == "Ordinary") {
        // 已经加入不再显示列表，不再显示
        if(!Global->getNoshowAdNumber().contains(fileInfo.number)) {
            preShowTime  = QDateTime::fromString(Global->getShowAdsTime(),"yyyy-MM-dd hh-mm-ss");
            int intrev = 5;
            if (_localFile.interval > 0)
                intrev = _localFile.interval;
            int intervalS = intrev * 3600;//interval的单位是小时
            // 少10s，防止时间相同或者存时间时占用的时间,小于0防止改时间
            if(preShowTime.secsTo(curTime) > (intervalS - 10) || preShowTime.secsTo(curTime) < 0) {
                show = true;
            }
        }
    } else if(fileInfo.type == "Holiday") {
        // 已经加入不再显示列表
        preShowTime  = QDateTime::fromString(Global->getShowAdsTime(),"yyyy-MM-dd hh-mm-ss");
        if(Global->getNoshowAdNumber().contains(fileInfo.number)) {
            int intervalS = 24 * 3600;//已经在显示列表时，24小时后显示
            // 少10s，防止时间相同或者存时间时占用的时间,小于0防止改时间
            if(preShowTime.secsTo(curTime) > (intervalS - 10) || preShowTime.secsTo(curTime) < 0) {
                show = true;
            }
        } else {
            int intrev = 5;
            if (_localFile.interval > 0)
                intrev = _localFile.interval;
            int intervalS = intrev * 3600;//interval的单位是小时
            // 少10s，防止时间相同或者存时间时占用的时间,小于0防止改时间
            if(preShowTime.secsTo(curTime) > (intervalS - 10) || preShowTime.secsTo(curTime) < 0) {
                show = true;
            }
        }

    } else if(fileInfo.type == "Upgrade"){
        if(Global->getNoshowAdNumber().contains(fileInfo.number)) {
            show = false;
        } else {
            show = true;
        }
    } else if(fileInfo.type == "DVD"){
        preShowTime  = QDateTime::fromString(Global->getShowAdsTime(),"yyyy-MM-dd hh-mm-ss");
        int intervalS = _adsConInfo.dvdConfig.interval * 60;
        // 小于0防止改时间
        if(preShowTime.secsTo(curTime) > intervalS || preShowTime.secsTo(curTime) < 0) {
            show = true;
        }
    } else if(fileInfo.type == "Video") {
        preShowTime  = QDateTime::fromString(Global->getShowAdsTime(),"yyyy-MM-dd hh-mm-ss");
        int intervalS = _adsConInfo.convertConfig.interval * 60;
        // 小于0防止改时间

        if(preShowTime.secsTo(curTime) > intervalS || preShowTime.secsTo(curTime) < 0) {
            show = true;
        }
    } else if(fileInfo.type == "iPhone") {
        preShowTime  = QDateTime::fromString(Global->getShowAdsTime(),"yyyy-MM-dd hh-mm-ss");
        int intervalS = _adsConInfo.iPhoneConfig.interval * 60;
        // 小于0防止改时间
        if(preShowTime.secsTo(curTime) > intervalS || preShowTime.secsTo(curTime) < 0) {
            show = true;
        }
    }

    return show;
}

void ADLogic::startShowAd()
{
    // 推送策略
    /*
     * 间隔时间：
     * Upgrade
     * a.第一次启动先推送优先级：Upgrade>Holiday>Ordinary;
     * b.间隔时间启动推送优先级：Holiday = Ordinary
     * c.事件触发：Upgrade 版本低于 DVD 使用dvd Video iPhone 5到10S后
     *
     */
    int intrev = 5;
    if (_localFile.interval > 0)
        intrev = _localFile.interval;
    _autoPushTimer->start(intrev * 3600*1000);

    if(_localFile.noadFlag)
        return;

    QList<adFile> showFile;
    showFile.clear();
    // 第一次启动 每次打开都会显示
    if(Global->_advertFirst){
        Global->_advertFirst = false;
        // 先取热点，如果没有，则取普通
        for(int i=0;i<_localFile.adfileList.size();i++) {
            if(_localFile.adfileList.at(i).type == "Holiday" && checkAdvertData(_localFile.adfileList.at(i).number)) {
                // 如果已经加入不再显示列表，检查是否满一天
                if(Global->getNoshowAdNumber().contains(_localFile.adfileList.at(i).number)) {
                    if(isShowTimeOut(_localFile.adfileList.at(i)))
                        showFile.append(_localFile.adfileList.at(i));
                } else {
                    showFile.append(_localFile.adfileList.at(i));
                }
            }
        }
        if(showFile.size() == 0) {
            for(int i=0;i<_localFile.adfileList.size();i++) {
                if(_localFile.adfileList.at(i).type == "Ordinary"
                   && !Global->getNoshowAdNumber().contains(_localFile.adfileList.at(i).number)
                   && checkAdvertData(_localFile.adfileList.at(i).number)) {
                    showFile.append(_localFile.adfileList.at(i));
                }
            }
        }
    } else {
        // 第二次时，需要看是否在不再显示列表，如果在并且是Holiday类型并且上次显示时间大于1天，则显示
        for(int i=0;i<_localFile.adfileList.size();i++) {
            if(_localFile.adfileList.at(i).type == "Holiday" || _localFile.adfileList.at(i).type == "Ordinary") {
                if(isShowTimeOut(_localFile.adfileList.at(i)) && checkAdvertData(_localFile.adfileList.at(i).number)) {
                    showFile.append(_localFile.adfileList.at(i));
                }
            }
        }
    }
    adFile adInfo;
    bool isFinish = false;
    if(showFile.size() > 0){
        int probCount = 0;
        for(int i=0;i<showFile.size();i++) {
            probCount += showFile.at(i).prob;
        }
        int rand = qrand()% probCount;
        int prob = 0;
        for(int i=0;i<showFile.size();i++) {
            prob += showFile.at(i).prob;
            if(rand < prob) {
                adInfo = showFile.at(i);
                isFinish = true;
                break;
            }
        }
    }
    if(isFinish) {
        _ggWidget->startShow(adInfo.number,adInfo.type);
    }
}

void ADLogic::startAdByEventType(QString type, QString winKey)
{
    QList<adFile> adsList;
    adFile adInfo;
    adsList.clear();
    bool isFinish = false;
    for(int i=0;i<_localFile.adfileList.size();i++) {
        if(_localFile.adfileList.at(i).type == type
           && isShowTimeOut(_localFile.adfileList.at(i))
           && checkAdvertData(_localFile.adfileList.at(i).number)
           && _localFile.adfileList.at(i).language == _adLanguage) {
            adsList.append(_localFile.adfileList.at(i));
        }
    }
    if(adsList.size() == 0)
        return;
    if(type == "Upgrade" || type == "DVD" || type == "iPhone") {
        adInfo = adsList.at(0);
        isFinish = true;
    } else {
        for(int i=0;i<adsList.size();i++) {
            if(QString::compare(getAdWinKey(adsList.at(i).number),winKey,Qt::CaseInsensitive) == 0) {
                adInfo = adsList.at(i);
                isFinish = true;
                break;
            }
        }
    }
    if(isFinish) {
        // 事件广告，先关闭推送广告定时器
        _autoPushTimer->stop();
        int intrev = 5;
        if (_localFile.interval > 0)
            intrev = _localFile.interval;
        _autoPushTimer->start(intrev * 3600*1000);
        _ggWidget->startShow(adInfo.number,adInfo.type);
    }
}

void ADLogic::languageChange()
{
    if(_adLanguage != Global->getLanguage()) {
        _adLanguage = Global->getLanguage();
        _useFixedWeb = false;
        getLocalAdFile();
        getAdFileByWeb();
    }
}

void ADLogic::initAdini()
{
    // 初始化
    _adsConInfo.iPhoneConfig.interval   = 5;
    _adsConInfo.dvdConfig.interval      = 5;

    convertFormat convertFmt;
    convertFmt.mediaFormat = "mp4";
    convertFmt.videoFormat = "h264";
    convertFmt.audioFormat = "aac";
    _adsConInfo.convertConfig.conMediaList.append(convertFmt);
    _adsConInfo.convertConfig.fileSize                       = 1610612736; // 1.5*1024*1024*1024
    _adsConInfo.convertConfig.resolution.setHeight(1080);
    _adsConInfo.convertConfig.resolution.setWidth(1920);
    _adsConInfo.convertConfig.interval                       = 15;

    // 加载本地配置文件
    bool loadOk = getLocalAdini();
    QDateTime curTime       = QDateTime::currentDateTime();
    QDateTime configUp      = QDateTime::fromString(Global->getAdiniUpTime(),"yyyy-MM-dd hh-mm-ss");
    if(configUp.daysTo(curTime) > 7 || configUp.daysTo(curTime) < 0 || !loadOk) {
        // 开始从服务器上更新
        getWebAdini();
    }
}

QList<LocalPreference> ADLogic::analyPreferenceInfo(QByteArray bytes)
{
    QList<LocalPreference> prefList;
    prefList.clear();
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(bytes, &error);
    if(error.error == QJsonParseError::NoError) {
        QJsonObject json_object = jsonDoc.object();
        QJsonArray prefList_object;
        prefList_object = json_object.value("pref_list").toArray();
        for(int i=0;i<prefList_object.count();i++) {
            LocalPreference prefInfo;
            prefInfo.adType     = prefList_object.at(i).toObject().value("preference_type").toString();
            prefInfo.adCount    = prefList_object.at(i).toObject().value("preference_count").toInt();
            prefList.append(prefInfo);
        }
    }
    return prefList;
}

void ADLogic::savePreference()
{
    if(_localPreference.size() > 0) {
        QJsonArray preferenceList;// 文字
        for(int i=0;i<_localPreference.count();i++) {
            QJsonObject preferenceObj;
            preferenceObj.insert("preference_type",      _localPreference.at(i).adType);
            preferenceObj.insert("preference_count",     _localPreference.at(i).adCount);
            preferenceList.append(preferenceObj);
        }
        QJsonObject preference;
        preference.insert("pref_list", preferenceList);

        QJsonDocument document;
        document.setObject(preference);
        QByteArray byte_array = document.toJson(QJsonDocument::Indented);

        QString savePath = _adFolderpath + "/adlocal.xml";
        QFile fi2(savePath);
        if(fi2.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        {
            fi2.write(byte_array);
            fi2.flush();
            fi2.close();
        }
    }
}

QString ADLogic::getPrefType()
{
    /* 先统计用户偏好，如果没有，则都为10，如果有一项大于10，则获得最大的值来进行计算
     *
     */
    // 获取最大的用户偏好值
    int balanceVaule = -1;
    for(int i=0;i<_localPreference.size();i++) {
        if(_localPreference.at(i).adCount > balanceVaule) {
            balanceVaule = _localPreference.at(i).adCount;
        }
    }
    QMap<QString,int> typePref;
    for(int i=0;i<_localPreference.size();i++) {
        int pref = 10;
        if(balanceVaule > 10) {
            if(_localPreference.at(i).adCount < balanceVaule) {
                pref += _localPreference.at(i).adCount / balanceVaule * 10;
            } else {
                pref += 10;
            }
        } else {
            pref =_localPreference.at(i).adCount;
        }
        typePref.insert(_localPreference.at(i).adType,pref);
    }

    for(int i=0;i<_localFile.adfileList.size();i++) {
        if(checkAdvertData(_localFile.adfileList.at(i).number)) {
            bool isExit = false;
            for(int j=0;j<_localPreference.size();j++) {
                if(QString::compare(_localPreference.at(j).adType,_localFile.adfileList.at(i).type,Qt::CaseInsensitive) == 0) {
                    isExit = true;
                    break;
                }
            }

            if(!isExit) {
                typePref.insert(_localFile.adfileList.at(i).type,10);
                LocalPreference newPref;
                newPref.adCount = 10;
                newPref.adType  = _localFile.adfileList.at(i).type;
                _localPreference.append(newPref);
                // 保存到xml
                savePreference();
            }
        }
    }

    int prefCount = 0;
    QMap<QString,int>::iterator preit; //遍历map
    for(preit = typePref.begin(); preit != typePref.end(); ++preit) {
        prefCount+=preit.value();
    }

    QString prefType = "";
    if(prefCount > 0) {
        // 获取所有的权重值来进行随机
        int prefValue = qrand()% (prefCount);
        int typeValue = 0;

        QMap<QString,int>::iterator it; //遍历map
        for(it = typePref.begin(); it != typePref.end(); ++it) {

            typeValue += it.value();
            if(prefValue <= typeValue || it == typePref.end()) {
                prefType = it.key();
                break;
            }
        }
    }
    return prefType;
}

QByteArray ADLogic::encodeAes(const QByteArray &key, const QByteArray &data)
{
    uchar iv [AES_BLOCK_SIZE+1] = "L+\\~f4,Ir)b$=pkf";

    AES_KEY AesKey;
    if(AES_set_encrypt_key((const uchar *)key.constData(), 128, &AesKey) < 0)//设置加密密钥
    {
        return "";
    }

    int len = data.length();
    if(len % AES_BLOCK_SIZE != 0)
        len = (len/AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;

    //补齐16的倍数
    QByteArray data2(data);
    data2.append(len - data2.length(), '\0');

    uchar out[len + 1] = {'\0'};

    AES_cbc_encrypt((uchar *)data2.data(),
                    (uchar *)out,
                    len, &AesKey,
                    iv, AES_ENCRYPT);

    return QByteArray((char*)out, len);
}

QByteArray ADLogic::decodeAes(const QByteArray &key, const QByteArray &data)
{
    uchar iv [AES_BLOCK_SIZE+1] = "L+\\~f4,Ir)b$=pkf";

    AES_KEY AesKey;
    if(AES_set_decrypt_key((const uchar *)key.constData(), 128, &AesKey) < 0)//设置加密密钥
    {
        return "";
    }

    int len = data.length();
    if(len % AES_BLOCK_SIZE != 0)
        len = (len/AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;

    //补齐16的倍数
    QByteArray data2(data);
    data2.append(len - data2.length(), '\0');

    char out[len + 1] = {'\0'};

    AES_cbc_encrypt((uchar *)data2.data(),
                    (uchar *)out,
                    len, &AesKey,
                    iv, AES_DECRYPT);

    return QByteArray(out, len);
}

QList<LocalPreference> ADLogic::getPreferInfoence()
{
    QList<LocalPreference> localList;
    localList.clear();
    QString adPath = _adFolderpath + "/adlocal.xml";
    QFile file(adPath);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray jsonByte = file.readAll();
        localList = analyPreferenceInfo(jsonByte);
    }
    return localList;
}
void ADLogic::httpAreaRead()
{
    QVariant status_code = _areaReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(status_code.toInt() != 200)
        return;
    _adArea = _areaReply->readAll();
}

void ADLogic::httpAreaFinish()
{
    QVariant status_code = _areaReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(status_code.toInt() == 200) {
        QJsonParseError error;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(_adArea, &error);
        if(error.error == QJsonParseError::NoError) {
            QJsonObject json_object = jsonDoc.object();
            QString area = json_object.value("cc").toString();
            Global->setLocalArea(area);
            Global->setLocalAreaTime(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
        }
    }
}

void ADLogic::httpWebReadyRead()
{
    QVariant status_code = _handReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(status_code.toInt() != 200)
        return;
    _adData = _handReply->readAll();
}

void ADLogic::httpWebFinished()
{
    _downAdList.clear();
    QVariant status_code = _handReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(status_code.toInt() == 200) {
        _iswebFinish = true;
        Global->saveAdsConfigTime(QDateTime::currentDateTime());
        _webFile = analyJsonByByteArray(_adData);
    } else {
        getAdFileByFixedWeb();
        return;
    }

    if(_iswebFinish)
        _downAdList = comparedLocalOrWeb();

    if(_downAdList.size() > 0) {
        startDownWebFile(_downAdList.at(0).url);
    }
}

void ADLogic::httpAdsReadyRead()
{
    QVariant status_code = _adsConfigReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(status_code.toInt() != 200)
        return;
    _adsConfigData = _adsConfigReply->readAll();
}

void ADLogic::httpAdsFinished()
{
    QVariant status_code = _adsConfigReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(status_code.toInt() == 200) {
        readWebAdini(QByteArray::fromBase64(_adsConfigData));
        writeLocalAdini();
        Global->saveAdiniUpTime(QDateTime::currentDateTime());
    } else {
        getOldWebAdini();
    }
}

void ADLogic::webDownFinish()
{
    _downFile->flush();
    _downFile->close();
    delete _downFile;
    _downFile = 0;
    QVariant status_code = _downReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(status_code.toInt() == 200) {
        _downReply->deleteLater();
        _downReply = NULL;
        QString toPath = Global->adFloderPath() + "/" + Global->getLanguage() + "/";
        if(QFileInfo::exists(_downFilePath)) {
            if(Global->Extract(_downFilePath,toPath)) {//解压缩,会自动覆盖文件
                downFinish(_downUrl);
            }
        }
    } else {
        downFail(_downUrl);
    }
}

void ADLogic::webDownRead()
{
    QVariant status_code = _downReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(status_code.toInt() != 200)
        return;
    if (_downFile) {
        _downFile->write(_downReply->readAll());  //如果文件存在，则写入文件
    }
}

void ADLogic::autoPushTimerOut()
{
    _useFixedWeb = false;
    _autoPushTimer->stop();
    startShowAd();
}

void ADLogic::adUpConfigOut()
{
    getLocalAdFile();
    QDateTime configUp      = QDateTime::fromString(Global->getAdsConfigTime(),"yyyy-MM-dd hh-mm-ss");
    if(configUp.daysTo(QDateTime::currentDateTime()) > 1 || configUp.daysTo(QDateTime::currentDateTime()) < 0 || _localFile.adfileList.size() < 6) {
        getAdFileByWeb();
    }
}

void ADLogic::openUrlSlt(QString type)
{
    // 点击了关键字
    if(type.isEmpty())
        return;
    bool isExit = false;
    for(int i=0;i<_localPreference.size();i++) {
        if(QString::compare(type,_localPreference.at(i).adType,Qt::CaseInsensitive) == 0){
            LocalPreference local;
            local.adType    = type;
            local.adCount   = _localPreference.at(i).adCount + 1;
            _localPreference.replace(i,local);
            isExit = true;
            break;
        }
    }
    if(!isExit) {
        LocalPreference local;
        local.adType    = type;
        local.adCount   = 2;        // 默认为1
        _localPreference.append(local);
    }
    // 保存到xml
    savePreference();
}

void ADLogic::appleNotifySlt()
{
    startAdByEventType("iPhone","");
}

void ADLogic::playMediaChange(MEDIAINFO mediainfo)
{
    if(mediainfo.title.isEmpty())
        return;
    if(mediainfo.mediaType == 1) {
        if(!compareFormat(mediainfo)) {
            startAdByEventType("Video","mp4");
        } else if(comparedSize(LibData->getVideoStream(mediainfo).resolution,_adsConInfo.convertConfig.resolution)) {
            startAdByEventType("Video","pro");
        } else if(mediainfo.filesize > _adsConInfo.convertConfig.fileSize) {
            startAdByEventType("Video","size");
        } else {
            startAdByEventType("Video","qt");
        }
    }
}
