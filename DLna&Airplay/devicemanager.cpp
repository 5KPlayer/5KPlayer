#include "devicemanager.h"

#include "openfile.h"
#include "dymediaplayer.h"
#include "controlmanager.h"
#include "dlnadevice.h"

#include <QDebug>
#include <QDomDocument>
#include <QFileInfo>
#include <QNetworkReply>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QTimer>
#include "playermain.h"

DeviceManager::DeviceManager(QObject *parent) : QObject(parent)
{
    _dev_state  = "NO_MEDIA_PRESENT";
    _trans_URL  = "";
    _trans_Urls.clear();
    _isM3u = false;
    _seekPos = 0;
}

void DeviceManager::setDevState(QString state)
{
    _dev_state = state;
}

QString DeviceManager::getDevState()
{
    if(MediaPlayer->state() == DYVlc::Paused) {
        _dev_state = "PAUSED_PLAYBACK";
    } else if(MediaPlayer->state() == DYVlc::Playing) {
        _dev_state = "PLAYING";
    } else if(MediaPlayer->state() == DYVlc::Stopped) {
        _dev_state = "STOPPED";
    } else {
        _dev_state = "NO_MEDIA_PRESENT";
    }
    return _dev_state;
}

void DeviceManager::setTransing(QString url)
{
    if(_trans_URL != url) {
        _dev_state = "Transing";
    }

    QFileInfo file(url);

    if(file.completeSuffix() == "m3u") {
        _isM3u = true;
        _trans_Urls = getM3uPaths(url);
        _trans_URL = "";
    } else {
        _isM3u = false;
        _seekPos = 0;
        _trans_Urls.clear();
        _trans_URL = url;
    }
}

char *DeviceManager::createEventLastChange(int type, char *actionName, QList<actionKey_Value> args)
{
    char* event = NULL;
    QString eventAttri = "";
    if(type == 0) { // xmlns="urn:schemas-upnp-org:metadata-1-0/RCS/"
        eventAttri = "urn:schemas-upnp-org:metadata-1-0/RCS/";
    } else if(type) { //xmlns="urn:schemas-upnp-org:metadata-1-0/AVT/"
        eventAttri = "urn:schemas-upnp-org:metadata-1-0/AVT/";
    }
    QDomDocument eventXml;
    QDomElement eventNode = eventXml.createElement("Event");
    eventNode.setAttribute("xmlns",eventAttri);
    QDomElement InstanceIDNode = eventXml.createElement("InstanceID");
    InstanceIDNode.setAttribute("val","0");

    QDomElement actionDNode = eventXml.createElement(actionName);
    for(int i=0;i<args.count();i++) {
        actionDNode.setAttribute(args.at(i).key,args.at(i).value);
    }
    InstanceIDNode.appendChild(actionDNode);
    eventNode.appendChild(InstanceIDNode);
    eventXml.appendChild(eventNode);
    QByteArray xmlByte = eventXml.toByteArray();

    event = xmlByte.data();
    return event;
}

void DeviceManager::deviceSeek(QString time)
{
    if(time.contains(":")) {
        int relTime = CONTROL->QStringTime2Int(time);
        emit deviceSeekPoint(relTime);
    } else {
        if(_trans_Urls.count() > 0) {
            _isM3u      = true;
            _seekPos    = time.toInt();
        }
    }
}

void DeviceManager::deviceTransportState(QString state)
{

    qDebug() << "deviceTransportState:" << state;
    QString url = _trans_URL;
    if(_dev_state == "PAUSED_PLAYBACK") {
        url = "";
    }
    if(state == "PLAYING") {
        if(_isM3u) {
            if(_seekPos > 0) {
                if(_seekPos - 1 < _trans_Urls.count())
                    _trans_URL = _trans_Urls.at(_seekPos -1);
                else
                    _trans_URL = _trans_Urls.at(_trans_Urls.count() -1);
            } else {
                _trans_URL = _trans_Urls.at(0);
            }

            url = _trans_URL;
        }
    }

    _dev_state = state;
    emit deviceTransState(state,url);
}

void DeviceManager::deviceMute()
{
    PMW->toggleMute();
}

void DeviceManager::deviceVolumeChange(int volume)
{
    // send lastChange msg
    QList<actionKey_Value> volumeList;
    volumeList.clear();
    actionKey_Value channel;
    actionKey_Value desiredVolume;
    strcpy(channel.key,         "channel");
    strcpy(channel.value,       "Master");
    strcpy(desiredVolume.key,   "val");
    strcpy(desiredVolume.value, CONTROL->QString2char(QString::number(volume)));
    volumeList.append(channel);
    volumeList.append(desiredVolume);
    AvDeviceLastChange(0,"Volume",volumeList);
}

void DeviceManager::devicePlayChange()
{
    QList<actionKey_Value> playList;
    playList.clear();
    actionKey_Value val;
    strcpy(val.key,   "val");
    strcpy(val.value, "PLAYING");
    playList.append(val);
    AvDeviceLastChange(1,"TransportState",playList);
}

void DeviceManager::devicePaushChange()
{
    QList<actionKey_Value> paushList;
    paushList.clear();
    actionKey_Value val;
    strcpy(val.key,   "val");
    strcpy(val.value, "PAUSED_PLAYBACK");
    paushList.append(val);
    AvDeviceLastChange(1,"TransportState",paushList);
}

void DeviceManager::deviceTransIngChange()
{
    QList<actionKey_Value> transingList;
    transingList.clear();
    actionKey_Value val;
    strcpy(val.key,   "val");
    strcpy(val.value, "TRANSITIONING");
    transingList.append(val);
    AvDeviceLastChange(1,"TransportState",transingList);
}

void DeviceManager::deviceStopChange()
{
    QList<actionKey_Value> stopList;
    stopList.clear();
    actionKey_Value val;
    strcpy(val.key,   "val");
    strcpy(val.value, "STOPPED");
    stopList.append(val);
    AvDeviceLastChange(1,"TransportState",stopList);
}

void DeviceManager::deviceNo_MediaChange()
{
    QList<actionKey_Value> no_mediaList;
    no_mediaList.clear();
    actionKey_Value val;
    strcpy(val.key,   "val");
    strcpy(val.value, "NO_MEDIA_PRESENT");
    no_mediaList.append(val);
    AvDeviceLastChange(1,"TransportState",no_mediaList);
}

void DeviceManager::deviceMuteChange(bool mute)
{
    QList<actionKey_Value> muteList;
    muteList.clear();
    actionKey_Value channel;
    actionKey_Value muteVolume;
    strcpy(channel.key,         "channel");
    strcpy(channel.value,       "Master");
    strcpy(muteVolume.key,   "val");
    strcpy(muteVolume.value, mute?"1":"0");
    muteList.append(channel);
    muteList.append(muteVolume);
    AvDeviceLastChange(0,"Mute",muteList);
}

void DeviceManager::deviceUnCon()
{
    if(_isM3u) {
        _isM3u      = false;
        _seekPos    = -1;
    }
}

QStringList DeviceManager::getM3uPaths(QString m3u)
{
    QStringList m3uPaths;
    m3uPaths.clear();
    QUrl url(m3u);
    QEventLoop loop;
    QNetworkAccessManager manager;
    QTimer time;

    QNetworkReply *reply = manager.get(QNetworkRequest(url));

    QObject::connect(&time,SIGNAL(timeout()),&loop,SLOT(quit()));
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    time.start(3000);
    loop.exec();

    QString paths = QString(reply->readAll());
    QStringList list = paths.split("\r\n");
    for(int i=0;i<list.count();i++) {
        if(list.at(i).startsWith("http:"))
            m3uPaths.append(list.at(i));
    }
    return m3uPaths;
}

QString DeviceManager::getNextM3u()
{
    QString m3uPath = "";
    if(_isM3u && _seekPos != -1) {
        if(_trans_Urls.count() > _seekPos)
            m3uPath = _trans_Urls.at(_seekPos);
        _seekPos++;
    }
    return m3uPath;
}

bool DeviceManager::getIsM3u()
{
    return _isM3u;
}

