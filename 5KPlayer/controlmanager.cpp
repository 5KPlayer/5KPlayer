#include "controlmanager.h"

#include "dlnactrlpoint.h"
#include "dlnadevice.h"
#include "z_httpserver.h"
#include "ixml.h"
#include "dymediaplayer.h"
#include "librarydata.h"
#include "globalarg.h"
#include "openfile.h"
#include "devicemanager.h"

#include <QTimer>
#include <QDomDocument>
#include <QLabel>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QApplication>
#include <QMessageBox>
#include <QScreen>
#include "dlnacommon.h"
#include <QUuid>
#include <QApplication>
#include <QDomDocument>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QHostInfo>

ControlManager::ControlManager(QObject *parent)
{
    _pointTimer     = new QTimer(this);
    _stopTimer      = new QTimer(this);
    _ctrlPlayTimer  = new QTimer(this);
    _stopTimer->setSingleShot(true);
    _ctrlPlayTimer->setSingleShot(true);
    _ctrlPlayTimer->setInterval(600);
    _stopTimer->setInterval(5000);
    _curDevUDN  = "";
    _selDevUDN  = "";
    _curFilePath= "";
    _currentUrl = "";
    _curPlayPoint   = "00:00:00";
    _curState   = "STOPPED";
    _selUdnDel  = false;
    _lockStop   = false;
    _playSource = false;
    connect(_stopTimer,SIGNAL(timeout()),SLOT(stopTimeOut()));
    connect(_pointTimer,SIGNAL(timeout()),SLOT(pointTimeOut()));
    connect(_ctrlPlayTimer,SIGNAL(timeout()),SLOT(playTimeOut()));
}

ControlManager::~ControlManager()
{
}

QString ControlManager::createUUid()
{
    QString uuid = QUuid::createUuid().toString().replace(QRegExp("(\\{)|(\\})"), "");
    return uuid;
}

QString ControlManager::getVideoSuffix(QString httpType)
{
    if(_contentVideoType.isEmpty()) {
        //video
        _contentVideoType["asf"]   = "video/x-ms-asf";
        _contentVideoType["asx"]   = "video/x-ms-asf";
        _contentVideoType["avi"]   = "video/avi";
        _contentVideoType["ivf"]   = "video/x-ivf";
        _contentVideoType["m1v"]   = "video/x-mpeg";
        _contentVideoType["m2v"]   = "video/x-mpeg";
        _contentVideoType["m4e"]   = "video/mpeg4";
        _contentVideoType["movie"] = "video/x-sgi-movie";
        _contentVideoType["mp2v"]  = "video/mpeg";
        _contentVideoType["mp4"]   = "video/mpeg4";
        _contentVideoType["mpa"]   = "video/x-mpg";
        _contentVideoType["mpe"]   = "video/x-mpeg";
        _contentVideoType["mpeg"]  = "video/mpg";
        _contentVideoType["mpg"]   = "video/mpg";
        _contentVideoType["mps"]   = "video/x-mpeg";
        _contentVideoType["mpv"]   = "video/mpg";
        _contentVideoType["mpv2"]  = "video/mpeg";
        _contentVideoType["rv"]    = "video/vnd.rn-realvideo";
        _contentVideoType["wm"]    = "video/x-ms-wm";
        _contentVideoType["wmv"]   = "video/x-ms-wmv";
        _contentVideoType["wmx"]   = "video/x-ms-wmx";
        _contentVideoType["wvx"]   = "video/x-ms-wvx";
        _contentVideoType["mp4"]   = "video/mp4";
        _contentVideoType["mov"]   = "video/quicktime";
    }
    return _contentVideoType.key(httpType, "Unknown");
}

QString ControlManager::getAudioSuffix(QString httpType)
{
    if(_contentAudioType.isEmpty()) {
        // audio
        _contentAudioType["acp"]   = "audio/x-mei-aac";
        _contentAudioType["aif"]   = "audio/aiff";
        _contentAudioType["aiff"]  = "audio/aiff";
        _contentAudioType["aifc"]  = "audio/aiff";
        _contentAudioType["au"]    = "audio/basic";
        _contentAudioType["la1"]   = "audio/x-liquid-file";
        _contentAudioType["lavs"]  = "audio/x-liquid-secure";
        _contentAudioType["lmsff"] = "audio/x-la-lms";
        _contentAudioType["m3u"]   = "audio/mpegurl";
        _contentAudioType["midi"]  = "audio/mid";
        _contentAudioType["mid"]   = "audio/mid";
        _contentAudioType["mp2"]   = "audio/mp2";
        _contentAudioType["mp3"]   = "audio/mp3";
        _contentAudioType["mp4"]   = "audio/mp4";
        _contentAudioType["mnd"]   = "audio/x-musicnet-download";
        _contentAudioType["mp1"]   = "audio/mp1";
        _contentAudioType["mns"]   = "audio/x-musicnet-stream";
        _contentAudioType["mpga"]  = "audio/rn-mpeg";
        _contentAudioType["pls"]   = "audio/scpls";
        _contentAudioType["ra"]    = "audio/vnd.rn-realaudio";
        _contentAudioType["ram"]   = "audio/x-pn-realaudio";
        _contentAudioType["rmi"]   = "audio/mid";
        _contentAudioType["rmm"]   = "audio/x-pn-realaudio";
        _contentAudioType["rpm"]   = "audio/x-pn-realaudio-plugin";
        _contentAudioType["snd"]   = "audio/basic";
        _contentAudioType["wav"]   = "audio/wav";
        _contentAudioType["wax"]   = "audio/x-ms-wax";
        _contentAudioType["wma"]   = "audio/x-ms-wma";
        _contentAudioType["xpl"]   = "audio/scpls";
    }
    return _contentAudioType.key(httpType, "Unknown");
}

bool ControlManager::checkSendMediaExt(MEDIAINFO media)
{
    bool isSend = true;
    if(!_selDevUDN.isEmpty()) {
        deviceInfo devInfo = getSelSupInfo();

        if(media.mediaType == 0) {
            isSend = checkSendPathExt(media.filepath);
        } else {
            if(devInfo.deviceType == 2) { // 音频设备
                if(media.mediaType != 2) { // 不是音乐类型媒体
                    isSend = false;
                } else {
                    if(QString::compare(media.ext,"mp3",Qt::CaseInsensitive) == 0
                       || QString::compare(media.ext,"aac",Qt::CaseInsensitive) == 0) {
                        isSend = true;
                    } else {
                        isSend = devInfo.deviceAudioFormats.contains(media.ext,Qt::CaseInsensitive);
                    }
                }
            } else if(devInfo.deviceType == 1) { // 视频设备
                if(media.mediaType == 1) {
                    if(QString::compare(media.ext,"mp4",Qt::CaseInsensitive) == 0) {
                        isSend = true;
                    } else {
                        isSend = devInfo.deviceVideoFormats.contains(media.ext,Qt::CaseInsensitive);
                    }
                } else {
                    if(QString::compare(media.ext,"mp3",Qt::CaseInsensitive) == 0
                       || QString::compare(media.ext,"aac",Qt::CaseInsensitive) == 0) {
                        isSend = true;
                    } else {
                        isSend = devInfo.deviceAudioFormats.contains(media.ext,Qt::CaseInsensitive);
                    }
                }
            } else {
                if(QString::compare(media.ext,"mp4",Qt::CaseInsensitive) == 0
                   || QString::compare(media.ext,"mp3",Qt::CaseInsensitive) == 0) {
                    isSend = true;
                } else {
                    isSend = false;
                }
            }
        }
    }
    return isSend;
}

bool ControlManager::checkSendPathExt(QString path)
{
    bool isSend = true;
    if(!_selDevUDN.isEmpty()) {
        deviceInfo devInfo = getSelSupInfo();
        QFileInfo file(path);
        QString ext = file.suffix();
        if(devInfo.deviceType == 2) { // 音频设备
            if(QString::compare(ext,"mp3",Qt::CaseInsensitive) == 0
               || QString::compare(ext,"aac",Qt::CaseInsensitive) == 0) {
                isSend = true;
            } else {
                isSend = devInfo.deviceAudioFormats.contains(ext,Qt::CaseInsensitive);
            }
        } else if(devInfo.deviceType == 1){
            if(QString::compare(ext,"mp4",Qt::CaseInsensitive) == 0
               || QString::compare(ext,"mp3",Qt::CaseInsensitive) == 0
               || QString::compare(ext,"aac",Qt::CaseInsensitive) == 0) {
                isSend = true;
            } else {
                isSend = devInfo.deviceVideoFormats.contains(ext,Qt::CaseInsensitive)
                        || devInfo.deviceAudioFormats.contains(ext,Qt::CaseInsensitive);
            }
        } else {
            if(QString::compare(ext,"mp4",Qt::CaseInsensitive) == 0
               || QString::compare(ext,"mp3",Qt::CaseInsensitive) == 0) {
                isSend = true;
            } else {
                isSend = false;
            }
        }
    }
    return isSend;
}

void ControlManager::pointTimeOut()
{
    if(!_selDevUDN.isEmpty()) {
        AvCtrlPointSendGetPoint(getSelUDN());
    }
}

void ControlManager::stopTimeOut()
{
    qDebug() <<"stopTimeOut:" << _curState;
    _lockStop   = false;
    _playSource = false;
    // 播放不成功
    if(_curState == "STOPPED") {
        emit sendCtrlportState(_curState);
    }
}

void ControlManager::playTimeOut()
{
    qDebug() << "playTimeOut";
    if(_currentUrl.isEmpty()) {
        OpenFileDialog->openFile(_currentMedia.filepath);
        LibData->setPlayMediaInfo(_currentMedia);
    } else {
        OpenFileDialog->setPlayType(OPenFile::Live);
        MediaPlayer->playFile(_currentUrl, false);
        LibData->setPlayURLInfo(_currentUrl);
    }

    Device->deviceSeek(Global->_playCurTime);
}

QByteArray ControlManager::createDeviceXml()
{
    QString xmlFloder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    xmlFloder.append("/web");
    QFileInfo fileInfo(xmlFloder);
    if(!fileInfo.isDir()) {
        QDir dir;
        bool ok = dir.mkdir(xmlFloder);
    }
    // 拷贝文件
    QString avPath = xmlFloder;
    avPath.append("/AVTransport_SCPD.xml");
    QFileInfo avFile(avPath);
    if(!avFile.isFile())
        copyFileToPath(":/res/web/AVTransport_SCPD.xml",avPath,true);

    QString conPath = xmlFloder;
    conPath.append("/ConnectionManager_SCPD.xml");
    QFileInfo conFile(conPath);
    if(!conFile.isFile())
        copyFileToPath(":/res/web/ConnectionManager_SCPD.xml",conPath,true);

    QString rendPath = xmlFloder;
    rendPath.append("/RenderingControl_scpd.xml");
    QFileInfo rendFile(rendPath);
    if(!rendFile.isFile())
        copyFileToPath(":/res/web/RenderingControl_scpd.xml",rendPath,true);

    QDomDocument deviceXml;
    QString xmlHeader( "version=\"1.0\"" );
    deviceXml.appendChild(deviceXml.createProcessingInstruction("xml", xmlHeader));
    QDomElement rootNode = deviceXml.createElement("root");
    rootNode.setAttribute("xmlns","urn:schemas-upnp-org:device-1-0");

    QDomElement versionNode = deviceXml.createElement("specVersion");
    QDomElement majNode     = deviceXml.createElement("major");
    QDomElement minNode     = deviceXml.createElement("minor");
    QDomText textmajNode    = deviceXml.createTextNode("1");
    QDomText textminNode    = deviceXml.createTextNode("0");
    majNode.appendChild(textmajNode);
    minNode.appendChild(textminNode);
    versionNode.appendChild(majNode);
    versionNode.appendChild(minNode);

    QDomElement deviceNode      = deviceXml.createElement("device");
    QDomElement Udn             = deviceXml.createElement("UDN");
    QDomElement friendlyName    = deviceXml.createElement("friendlyName");
    QDomElement deviceType      = deviceXml.createElement("deviceType");
    QDomElement manufacturer    = deviceXml.createElement("manufacturer");
    QDomElement manufacturerURL = deviceXml.createElement("manufacturerURL");
    QDomElement modelName       = deviceXml.createElement("modelName");
    QDomElement modelURL        = deviceXml.createElement("modelURL");
    QDomElement dlna            = deviceXml.createElement("dlna:X_DLNADOC");
    dlna.setAttribute("xmlns:dlna","urn:schemas-dlna-org:device-1-0");
    _curDevUDN          = QString("uuid:").append(createUUid());
    QDomText UdnText    = deviceXml.createTextNode(_curDevUDN);
    QDomText dlnaText   = deviceXml.createTextNode("DMR-1.50");
    QDomText deviceText = deviceXml.createTextNode("urn:schemas-upnp-org:device:MediaRenderer:1");
    QString friendName  = Global->getDLnaDeviceName();
    QDomText friendlyText = deviceXml.createTextNode(friendName);
    QDomText facturerText = deviceXml.createTextNode("Digiarty");
    QDomText facturerURLText    = deviceXml.createTextNode("https://www.5kplayer.com");
    QDomText modelNameText  = deviceXml.createTextNode("5KPlayer Media Renderer");
    QDomText modelURLText   = deviceXml.createTextNode("https://www.5kplayer.com");

    dlna.appendChild(dlnaText);
    deviceType.appendChild(deviceText);
    manufacturer.appendChild(facturerText);
    manufacturerURL.appendChild(facturerURLText);
    modelName.appendChild(modelNameText);
    modelURL.appendChild(modelURLText);
    friendlyName.appendChild(friendlyText);
    Udn.appendChild(UdnText);

    QDomElement serverList      = deviceXml.createElement("serviceList");
    QDomElement randServer      = deviceXml.createElement("service");

    QDomElement randSerType     = deviceXml.createElement("serviceType");
    QDomText    randTypeText    = deviceXml.createTextNode("urn:schemas-upnp-org:service:RenderingControl:1");
    randSerType.appendChild(randTypeText);

    QDomElement randSerId       = deviceXml.createElement("serviceId");
    QDomText    randIDText      = deviceXml.createTextNode("urn:upnp-org:serviceId:RenderingControl");
    randSerId.appendChild(randIDText);

    QDomElement randSerConURL   = deviceXml.createElement("controlURL");
    QDomText    randConURLText  = deviceXml.createTextNode("/upnp/control/RenderingControl");
    randSerConURL.appendChild(randConURLText);

    QDomElement randSerEnvURL   = deviceXml.createElement("eventSubURL");
    QDomText    randEnvURLText  = deviceXml.createTextNode("/upnp/event/RenderingControl");
    randSerEnvURL.appendChild(randEnvURLText);

    QDomElement randSerSCPDURL  = deviceXml.createElement("SCPDURL");
    QDomText    randSCPDURLText = deviceXml.createTextNode("/RenderingControl_scpd.xml");
    randSerSCPDURL.appendChild(randSCPDURLText);

    randServer.appendChild(randSerType);
    randServer.appendChild(randSerId);
    randServer.appendChild(randSerConURL);
    randServer.appendChild(randSerEnvURL);
    randServer.appendChild(randSerSCPDURL);

    QDomElement transServer     = deviceXml.createElement("service");
    QDomElement transSerType     = deviceXml.createElement("serviceType");
    QDomText    transTypeText    = deviceXml.createTextNode("urn:schemas-upnp-org:service:AVTransport:1");
    transSerType.appendChild(transTypeText);

    QDomElement transSerId       = deviceXml.createElement("serviceId");
    QDomText    transIDText      = deviceXml.createTextNode("urn:upnp-org:serviceId:AVTransport");
    transSerId.appendChild(transIDText);

    QDomElement transSerConURL   = deviceXml.createElement("controlURL");
    QDomText    transConURLText  = deviceXml.createTextNode("/upnp/control/AVTransport");
    transSerConURL.appendChild(transConURLText);

    QDomElement transSerEnvURL   = deviceXml.createElement("eventSubURL");
    QDomText    transEnvURLText  = deviceXml.createTextNode("/upnp/event/AVTransport");
    transSerEnvURL.appendChild(transEnvURLText);

    QDomElement transSerSCPDURL  = deviceXml.createElement("SCPDURL");
    QDomText    transSCPDURLText = deviceXml.createTextNode("/AVTransport_SCPD.xml");
    transSerSCPDURL.appendChild(transSCPDURLText);

    transServer.appendChild(transSerType);
    transServer.appendChild(transSerId);
    transServer.appendChild(transSerConURL);
    transServer.appendChild(transSerEnvURL);
    transServer.appendChild(transSerSCPDURL);

    QDomElement connectServer   = deviceXml.createElement("service");
    QDomElement connectSerType     = deviceXml.createElement("serviceType");
    QDomText    connectTypeText    = deviceXml.createTextNode("urn:schemas-upnp-org:service:ConnectionManager:1");
    connectSerType.appendChild(connectTypeText);

    QDomElement connectSerId       = deviceXml.createElement("serviceId");
    QDomText    connectIDText      = deviceXml.createTextNode("urn:upnp-org:serviceId:ConnectionManager");
    connectSerId.appendChild(connectIDText);

    QDomElement connectSerConURL   = deviceXml.createElement("controlURL");
    QDomText    connectConURLText  = deviceXml.createTextNode("/upnp/control/ConnectionManager");
    connectSerConURL.appendChild(connectConURLText);

    QDomElement connectSerEnvURL   = deviceXml.createElement("eventSubURL");
    QDomText    connectEnvURLText  = deviceXml.createTextNode("/upnp/event/ConnectionManager");
    connectSerEnvURL.appendChild(connectEnvURLText);

    QDomElement connectSerSCPDURL  = deviceXml.createElement("SCPDURL");
    QDomText    connectSCPDURLText = deviceXml.createTextNode("/ConnectionManager_SCPD.xml");
    connectSerSCPDURL.appendChild(connectSCPDURLText);

    connectServer.appendChild(connectSerType);
    connectServer.appendChild(connectSerId);
    connectServer.appendChild(connectSerConURL);
    connectServer.appendChild(connectSerEnvURL);
    connectServer.appendChild(connectSerSCPDURL);

    serverList.appendChild(randServer);
    serverList.appendChild(transServer);
    serverList.appendChild(connectServer);

    deviceNode.appendChild(deviceType);
    deviceNode.appendChild(manufacturer);
    deviceNode.appendChild(manufacturerURL);
    deviceNode.appendChild(modelName);
    deviceNode.appendChild(modelURL);
    deviceNode.appendChild(friendlyName);
    deviceNode.appendChild(Udn);
    deviceNode.appendChild(dlna);
    deviceNode.appendChild(serverList);

    rootNode.appendChild(versionNode);
    rootNode.appendChild(deviceNode);
    deviceXml.appendChild(rootNode);

    QByteArray xmlByte;
    QTextStream stream(&xmlByte);// (&file);
    deviceXml.save(stream, 4);
    return xmlByte;
}

bool ControlManager::copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist)
{
    toDir.replace("\\","/");
        if (sourceDir == toDir){
            return true;
        }
        if (!QFile::exists(sourceDir)){
            return false;
        }
        QDir *createfile     = new QDir;
        bool exist = createfile->exists(toDir);
        if (exist){
            if(coverFileIfExist){
                createfile->remove(toDir);
            }
        }//end if

        if(!QFile::copy(sourceDir, toDir))
        {
            return false;
        }
        return true;
}

char *ControlManager::QString2char(QString str)
{
    char* ch = (char*) malloc(str.size()+1);
    QByteArray ba = str.toLatin1();
    strcpy(ch,ba.data());

    return ch;
}

char *ControlManager::QByteArray2char(QByteArray byte)
{
    char* ch;
    ch = (char*) malloc(sizeof(byte));
    ch = byte.data();
    return ch;
}

int ControlManager::charTime2Int(char *time)
{
    QStringList timeList = QString(time).split(":");
    QString h = "0";
    QString m = "0";
    QString s = "0";
    if(timeList.count() == 3) {
        h = timeList.at(0);
        m = timeList.at(1);
        s = timeList.at(2);
    } else if(timeList.count() == 2) {
        m = timeList.at(1);
        s = timeList.at(2);
    }

    int ret = (h.toInt() * 3600 + m.toInt()*60 + s.toInt()) * 1000;
    return ret;
}

int ControlManager::QStringTime2Int(QString time)
{
    QStringList timeList = time.split(":");
    QString h = "0";
    QString m = "0";
    QString s = "0";
    if(timeList.count() == 3) {
        h = timeList.at(0);
        m = timeList.at(1);
        s = timeList.at(2);
    }

    int ret = (h.toInt() * 3600 + m.toInt()*60 + s.toInt()) * 1000;
    return ret;
}

char *ControlManager::Int2charTime(int value)
{
    int hour = value / 3600;
    int minute = (value%3600) / 60;
    int second = (value%3600)%60;
    QString hourStr   = QString::number(hour).rightJustified(2, '0');
    QString minuteStr = QString::number(minute).rightJustified(2, '0');
    QString secondStr = QString::number(second).rightJustified(2, '0');
    QString time = QString("%1:%2:%3").arg(hourStr).arg(minuteStr).arg(secondStr);
    return QString2char(time);
}

char* ControlManager::getBaseHttp(const char *location)
{
    QString http(location);
    int pos = http.indexOf("/",10);
    QString baseHttp = http.left(pos);
    return QString2char(baseHttp);
}

bool ControlManager::isSelUDN()
{
    return !_selDevUDN.isEmpty();
}

char* ControlManager::getSelUDN()
{
    return QString2char(_selDevUDN);
}

char *ControlManager::getCurUDN()
{
    return QString2char(_curDevUDN);
}

void ControlManager::setSelUDN(QString UDN)
{
    _selDevUDN = UDN;
}

deviceInfo ControlManager::getSelSupInfo()
{
    deviceInfo supInfo;
    int pos = -1;
    for(int i=0;i<_deviceList.count();i++) {
        if(strcmp(_deviceList.at(i).udn,QString2char(_selDevUDN)) == 0) {
            pos = i;
            break;
        }
    }
    if(pos != -1) {
        supInfo.deviceAudioFormats  = _deviceList.at(pos).deviceAudioFormats;
        supInfo.deviceVideoFormats  = _deviceList.at(pos).deviceVideoFormats;
        supInfo.deviceType          = _deviceList.at(pos).deviceType;
        strcpy(supInfo.friendlyName,_deviceList.at(pos).friendlyName);
        strcpy(supInfo.udn,_deviceList.at(pos).udn);
    }
    return supInfo;
}

void ControlManager::conDevice(QString UDN)
{
    // 如果是连接并且正在播放
    if(OpenFileDialog->curPlayType() == OPenFile::CONNECTION) {
        PMW->stopVlc(true);
        _selDevUDN = UDN;
        AvCtrlPointSendGetVolume(getSelUDN());  // 获取音量
        AvCtrlPointSendGetMute(getSelUDN());    // 获取是否静音
    } else {
        _selDevUDN = UDN;
        AvCtrlPointSendGetVolume(getSelUDN());  // 获取音量
        AvCtrlPointSendGetMute(getSelUDN());    // 获取是否静音
        if(MediaPlayer->state() == DYVlc::Playing ||
           MediaPlayer->state() == DYVlc::Paused) {
            // 播放的是投过来的文件
            if(LibData->getPlayURL().isEmpty() &&
               LibData->getPlayMediaInfo().filepath.isEmpty()) {
                PMW->stopVlc(true);
            } else {
                QString playPath = "";
                bool isUrl = false;
                if(!LibData->getPlayURL().isEmpty()) {
                    playPath = LibData->getPlayURL();
                    isUrl    = true;
                } else {
                    playPath = LibData->getPlayMediaInfo().filepath;
                }
                if(!checkSendPathExt(playPath) && !isUrl) {
                    PMW->stopVlc(true);
                    OpenFileDialog->openDLnaNoSup(playPath);
                } else {
                // 記錄播放文件名和進度
                    _lockStop = true;
                    QString playUrl;
                    if(OpenFileDialog->curPlayType() == OPenFile::Video ||
                       OpenFileDialog->curPlayType() == OPenFile::Music) {
                        _currentMedia   = LibData->getPlayMediaInfo();
                        playUrl         = LibData->getcurmediaPath();
                        _currentUrl     = "";
                        _stopTimer->start();
                    } else {
                        MEDIAINFO media;
                        _currentMedia   = media;
                        _currentUrl     = LibData->getPlayURL();
                        playUrl         = _currentUrl;
                        _stopTimer->start();
                    }

                    _curPlayPoint   = Global->_playCurTime;
                    qDebug() << "conDevice:" << playUrl << _curPlayPoint ;
                    MediaPlayer->stop();
                    Device->deviceStopChange();
                    if(!playUrl.isEmpty()) {
                        OpenFileDialog->setPlayType(OPenFile::CONNECTION);
                        LibData->setPlayMediaPath(playUrl);
                        PMW->startConnectedMode();

                        QString url;
                        if(_currentUrl.isEmpty()) {
                            url     = ZHttp->start(_localIp);
                            QString hash    = ZHttp->addFile(playUrl);
                            url.append("/").append(hash);
                        } else {
                            url = _currentUrl;
                        }

                        AvCtrlPointSendSetTransportURI(getSelUDN(),QString2char(url));
                    }
                }
            }
        }
    }
    emit sendUPNPCon(_selDevUDN.isEmpty());
    _clickUnCon = true;
}

void ControlManager::unconDevice()
{
    if(_clickUnCon) {
        _selUdnDel = false;
        if(_curState != "STOPPED") {
            _curState = "STOPPED";
            if(_stopTimer->isActive()) {
                _stopTimer->stop();
            }
            if(_pointTimer->isActive())
                _pointTimer->stop();
            AvCtrlPointSendStop(getSelUDN());
            PMW->closeConnectedMode();
            _selDevUDN  = "";
            _ctrlPlayTimer->start();
        }
        _selDevUDN = "";
        emit sendUPNPCon(_selDevUDN.isEmpty());
    }
}

void ControlManager::clickunDevice()
{
    _clickUnCon = false;
    _selUdnDel  = false;
    if(_curState != "STOPPED") {
        _curState = "STOPPED";
        if(_stopTimer->isActive()) {
            _stopTimer->stop();
        }
        stopGetPoint();
        AvCtrlPointSendStop(getSelUDN());
        PMW->closeConnectedMode();
        _selDevUDN  = "";
        _ctrlPlayTimer->start();
    }
    _selDevUDN = "";
    emit sendUPDevList(_deviceList,true);
    emit sendUPNPCon(_selDevUDN.isEmpty());
}

void ControlManager::clearSelDeviceInfo()
{
    if(!_selDevUDN.isEmpty()) {
        if(_curState != "STOPPED") {
            stopFile();
        }
        _selDevUDN = "";
        emit sendUPNPCon(_selDevUDN.isEmpty());
    }
}

bool ControlManager::getLockStop()
{
    return _lockStop;
}

bool ControlManager::getPlaySource()
{
    return _playSource;
}

void ControlManager::setLocalIp(QString localIp)
{
    _localIp = localIp;
}

bool ControlManager::getIsUrl()
{
    return !_currentUrl.isEmpty();
}

QString ControlManager::getCurPlayUrl()
{
    return _currentUrl;
}

MEDIAINFO ControlManager::getCurPlayMedia()
{
    return _currentMedia;
}

void ControlManager::startGetPoint()
{
    _pointTimer->start(1000);
}

void ControlManager::stopGetPoint()
{
    if(_curState != "STOPPED")
        AvCtrlPointSendStop(getSelUDN());
    if(_pointTimer->isActive())
        _pointTimer->stop();
}

void ControlManager::analyDeviceLastChange(char *change)
{/*
    qDebug() << "-----------------------\n"
             << change << "\n"
             << "------------------------\n";*/
    QDomDocument xml;
    xml.setContent(QString(change));
    QDomNode n = xml.firstChild();
    while (!n.isNull()) {
        QString val = getValForXML(n,"TransportState");
        QString url = getValForXML(n,"AVTransportURI");
        QString vol = getValForXML(n,"Volume");
        QString mute= getValForXML(n,"Mute");
        if(!val.isEmpty()) {
            if(val == "STOPPED") {
                if(url.isEmpty()) {
                    if(_curState != "STOPPED") {
                        _curState = val;
                        emit sendCtrlportState(_curState);
                    }
                }
            } else {
                _curState = val;
                emit sendCtrlportState(_curState);
            }
        }
        if(!vol.isEmpty()) {
            int volume = vol.toInt();
            emit sendCtrlVolumeChange(volume);
        }
        if(!mute.isEmpty()) {
            bool isMute = false;
            if(mute == "1" || mute == "true" || mute == "TRUE")
                isMute = true;
            emit sendCtrlMuteChange(isMute);
        }

//        qDebug() << "val:" << val
//                 << "url:" << url
//                 << "vol:" << vol;
        n = n.firstChild();
    }
//    free(change);
//    change = NULL;
}

void ControlManager::analyCtrlActionComplete(IXML_Document *xmlDoc, char *url)
{
//    if(strcmp(xmlDoc->n.firstChild->localName,"GetPositionInfoResponse") != 0) {
//        qDebug() << "analysisResultXml:" << xmlDoc->n.firstChild->localName
//                 << ixmlDocumenttoString(xmlDoc);
//    }
    if(strcmp(xmlDoc->n.firstChild->localName,"GetVolumeResponse") == 0) {
        int curVolume = atoi(getFirstDocumentItem(xmlDoc, "CurrentVolume"));
        emit sendCtrlVolumeChange(curVolume);
    } else if(strcmp(xmlDoc->n.firstChild->localName,"GetPositionInfoResponse") == 0) {
        // 这里是为了跳到本地播放的位置
        if(charTime2Int(getFirstDocumentItem(xmlDoc, "RelTime")) > 0) {
            if(_curPlayPoint != "00:00:00") {
                positionChange(QStringTime2Int(_curPlayPoint));
                _curPlayPoint = "00:00:00";
            }
        }

        int curTime     = charTime2Int(getFirstDocumentItem(xmlDoc, "RelTime"));
        int duration    = charTime2Int(getFirstDocumentItem(xmlDoc, "TrackDuration"));

        emit sendUpPoint(curTime,duration);

        if((curTime > 0) && (curTime == duration || (curTime + 2000) >= duration)) {
            _playSource = true;
        }

    } else if(strcmp(xmlDoc->n.firstChild->localName,"SetAVTransportURIResponse") == 0) {
        _curState = "TRANSITIONING";
        AvCtrlPointSendPlay(getSelUDN());
    } else if(strcmp(xmlDoc->n.firstChild->localName,"PlayResponse")== 0) {
        if(_curState == "PAUSED_PLAYBACK" || _curState == "TRANSITIONING") {
            _curState = "PLAYING";
            emit sendCtrlportState(_curState);
        }
    } else if(strcmp(xmlDoc->n.firstChild->localName,"PauseResponse")== 0) {
        if(_curState == "PLAYING") {
            _curState = "PAUSED_PLAYBACK";
            emit sendCtrlportState(_curState);
        }
    } else if(strcmp(xmlDoc->n.firstChild->localName,"StopResponse")== 0) {
        if(_curState != "STOPPED") {
            _curState = "STOPPED";
            emit sendCtrlportState(_curState);
        }
    } else if(strcmp(xmlDoc->n.firstChild->localName,"SetMuteResponse")== 0) {
        emit sendCtrlMuteChange(_curMute);
    } else if(strcmp(xmlDoc->n.firstChild->localName,"GetProtocolInfoResponse") == 0) {
        analyDeviceSupInfo(getFirstDocumentItem(xmlDoc, "Sink"),url);
    }
}

QString ControlManager::getValForXML(QDomNode node, QString name)
{
    QString val = "";
    while(!node.isNull()) {
        if (node.isElement()) {
            QDomElement e = node.toElement();
            if(e.tagName() == name) {
                val = e.attribute("val");
            }
        }
        node = node.nextSibling();
    }
    return val;
}

void ControlManager::analyDeviceSupInfo(QString info, QString url)
{
    deviceInfo supInfo;
    int pos = -1;
    QStringList infoList = info.split(",");
    foreach (QString sup, infoList) {
        QStringList supList = sup.split(":");
        if(supList.count() > 3) {
            QString suplog = supList.at(2);
            if(suplog.startsWith("video/")) {
                QString suffix = getVideoSuffix(suplog);
                if(suffix != "Unknown"
                   && !supInfo.deviceVideoFormats.contains(suffix)) {
                    supInfo.deviceVideoFormats.append(suffix);
                }
                supInfo.deviceType = 1;
            } else if(suplog.startsWith("audio/")) {
                QString suffix = getAudioSuffix(suplog);
                if(suffix != "Unknown"
                   && !supInfo.deviceAudioFormats.contains(suffix)) {
                    supInfo.deviceAudioFormats.append(suffix);
                }
                if(supInfo.deviceType == -1) {
                    supInfo.deviceType = 2;
                }
            }
        }
    }

    for(int i=0;i<_deviceList.count();i++) {
        if(url.startsWith(_deviceList.at(i).baseHttp)) {
            strcpy(supInfo.udn,_deviceList.at(i).udn);
            strcpy(supInfo.friendlyName,_deviceList.at(i).friendlyName);
            strcpy(supInfo.baseHttp,_deviceList.at(i).baseHttp);
            pos = i;
            break;
        }
    }

    if(pos != -1) {
        _deviceList.replace(pos,supInfo);
        emit sendUPDevList(_deviceList,false);
    }
}

int ControlManager::CtlvolumeChange(int volume)
{
    int ret = -1;
    if(!_selDevUDN.isEmpty()) {
        ret = AvCtrlPointSendSetVolume(getSelUDN(),volume);
    }
    return ret;
}

int ControlManager::CtrlSetMute(int mute)
{
    int ret;
    if(!_selDevUDN.isEmpty()) {
        _curMute = mute;
        ret = AvCtrlPointSendSetMute(getSelUDN(),mute);
    }
    return ret;
}

int ControlManager::positionChange(int proVol)
{
    int ret = -1;
    if(!_selDevUDN.isEmpty()) {
        ret = AvCtrlPointSendSeek(getSelUDN(),Int2charTime(proVol/1000));
    }
    return ret;
}

int ControlManager::playFilePath(QString filePath)
{
    _curPlayPoint = "00:00:00";
    qDebug() << "playFilePath:" << filePath << _curState;
    int ret = 0;
    QFileInfo fileInfo(filePath);
    _currentMedia.filepath  = filePath;
    _currentMedia.title     = fileInfo.completeBaseName();

    _currentUrl = "";

    _lockStop = true;
    _stopTimer->start();

    QString url     = ZHttp->start(_localIp);
    QString hash    = ZHttp->addFile(filePath);
    url.append("/").append(hash);
    ret = AvCtrlPointSendSetTransportURI(getSelUDN(),QString2char(url));
    return ret;
}

int ControlManager::playUrlPath(QString url)
{
    _curPlayPoint = "00:00:00";
    int ret = 0;
    MEDIAINFO media;
    _currentMedia   = media;

    _currentUrl = url;


    _lockStop = true;
    _stopTimer->start();

    ret = AvCtrlPointSendSetTransportURI(getSelUDN(),QString2char(url));
    return ret;
}

void ControlManager::playClick()
{
    if(!_selDevUDN.isEmpty()) {
        if(_curState == "PAUSED_PLAYBACK") {
            AvCtrlPointSendPlay(getSelUDN());
        } else {
            AvCtrlPointSendPause(getSelUDN());
        }
    }
}

int ControlManager::paushFile()
{
    int ret = -1;
    if(!_selDevUDN.isEmpty()) {
        ret = AvCtrlPointSendPause(getSelUDN());
    }
    return ret;
}

int ControlManager::stopFile()
{
    int ret = -1;
    _lockStop   = false;
    _playSource = false;
    _currentUrl = "";
    _curState   = "STOPPED";
    if(!_selDevUDN.isEmpty()) {
        ret = AvCtrlPointSendStop(getSelUDN());
        if(_stopTimer->isActive())
            _stopTimer->stop();
        if(_pointTimer->isActive())
            _pointTimer->stop();
    }
    PMW->closeConnectedMode();
    LibData->clearPlayMediaInfo();

    return ret;
}

void ControlManager::sendGetProtocolInfo(const char *udn)
{
    AvCtrlPointSendGetProtocolInfo(udn);
}

void ControlManager::addDevice(deviceInfo device)
{
    bool isExits = false;
    for(int i=0;i<_deviceList.count();i++) {
        if(strcmp(_deviceList.at(i).udn,device.udn) == 0) {
            isExits = true;
            break;
        }
    }
    if(!isExits) {
        if(strcmp(device.udn,QString2char(_curDevUDN)) != 0) {
            _deviceList.append(device);
            if(device.deviceType == -1) {
                emit sendGetDeviceSup(device);
            }
            emit sendUPDevList(_deviceList,true);
        }
    }
}

void ControlManager::delDevice(char *udn, bool isVerify)
{
    int pos = -1;
    _selUdnDel = false;
    if(strcmp(getSelUDN(),udn) == 0 && !isVerify) {
        _selUdnDel = true;
    }
    for(int i=0;i<_deviceList.count();i++) {
        if(strcmp(_deviceList.at(i).udn,udn) == 0) {
            pos = i;
            break;
        }
    }
    if(pos != -1) {
        _deviceList.removeAt(pos);
        emit sendUPDevList(_deviceList,true);
    }
}

bool ControlManager::getDevSelUdnFlag()
{
    return _selUdnDel;
}

QList<deviceInfo> ControlManager::getDeviceList()
{
    return _deviceList;
}

char* ControlManager::getDeviceSink()
{    
    QString device = "http-get:*:*:*,xbmc-get:*:*:*,http-get:*:audio/aac:*,"
                     "http-get:*:audio/ape:*,http-get:*:audio/mkv:*,http-get:*:audio/mpegurl:*,"
                     "http-get:*:audio/mpeg:*,http-get:*:audio/mpeg3:*,http-get:*:audio/mp3:*,"
                     "http-get:*:audio/mp4:*,http-get:*:audio/basic:*,http-get:*:audio/midi:*,"
                     "http-get:*:audio/ulaw:*,http-get:*:audio/ogg:*,http-get:*:audio/DVI4:*,"
                     "http-get:*:audio/G722:*,http-get:*:audio/G723:*,http-get:*:audio/G726-16:*,"
                     "http-get:*:audio/G726-24:*,http-get:*:audio/G726-32:*,http-get:*:audio/G726-40:*,"
                     "http-get:*:audio/G728:*,http-get:*:audio/G729:*,http-get:*:audio/G729D:*,"
                     "http-get:*:audio/G729E:*,http-get:*:audio/GSM:*,http-get:*:audio/GSM-EFR:*,"
                     "http-get:*:audio/L8:*,http-get:*:audio/L16:*,http-get:*:audio/LPC:*,"
                     "http-get:*:audio/MPA:*,http-get:*:audio/PCMA:*,http-get:*:audio/PCMU:*,"
                     "http-get:*:audio/QCELP:*,http-get:*:audio/RED:*,http-get:*:audio/VDVI:*,"
                     "http-get:*:audio/ac3:*,http-get:*:audio/vorbis:*,http-get:*:audio/speex:*,"
                     "http-get:*:audio/flac:*,http-get:*:audio/wav:*,http-get:*:audio/x-flac:*,"
                     "http-get:*:audio/x-aiff:*,http-get:*:audio/x-pn-realaudio:*,http-get:*:audio/x-realaudio:*,"
                     "http-get:*:audio/x-wav:*,http-get:*:audio/x-matroska:*,http-get:*:audio/x-ms-wma:*,"
                     "http-get:*:audio/x-mpegurl:*,"
                     "http-get:*:video/x-ms-asf:*,http-get:*:video/avi:*,http-get:*:video/x-ivf:*,"
                     "http-get:*:video/x-mpeg:*,http-get:*:video/mpeg4:*,http-get:*:video/x-sgi-movie:*,"
                     "http-get:*:video/video/mpeg:*,http-get:*:video/mpeg4:*,http-get:*:video/x-mpg:*,"
                     "http-get:*:video/x-mpeg:*,http-get:*:video/mpg:*,http-get:*:video/x-mpeg:*,"
                     "http-get:*:video/x-ms-wm:*,http-get:*:video/x-ms-wmv:*,http-get:*:video/x-ms-wmx:*,"
                     "http-get:*:video/x-ms-wvx:*,http-get:*:video/mov:*";
    char* ch = (char*) malloc(device.length()+1);
    QByteArray ba = device.toLatin1();

    strcpy(ch,ba.data());

    return ch;
}
