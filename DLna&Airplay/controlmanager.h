#ifndef CONTROLMANAGER_H
#define CONTROLMANAGER_H

#include <QObject>
#include "singleton.h"
#include "devstruct.h"
#include "ithread.h"
#include "dystructu.h"

class QDomNode;
class QTimer;
#define CONTROL ControlManager::getInstance()

class ControlManager: public QObject,public Singleton<ControlManager>
{
    Q_OBJECT
    friend class Singleton<ControlManager>;
    friend class QSharedPointer<ControlManager>;
public:

    ControlManager(QObject *parent = 0);
    ~ControlManager();

    QByteArray createDeviceXml();
    bool       copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist);

    char*   QString2char(QString str);
    char*   QByteArray2char(QByteArray byte);
    int     charTime2Int(char* time);
    int     QStringTime2Int(QString time);
    char*   Int2charTime(int value);
    char *getBaseHttp(const char * location);

    bool    isSelUDN();
    char*   getSelUDN();
    char*   getCurUDN();
    void    setSelUDN(QString UDN);

    deviceInfo getSelSupInfo();

    void    conDevice(QString UDN);         // 连接设备
    void    unconDevice();                  // 断开设备
    void    clickunDevice();

    void    clearSelDeviceInfo();           // 作为控制端时，有新的控制播放时，清除控制端信息

    bool    getLockStop();
    bool    getPlaySource();

    void    setLocalIp(QString localIp);

    bool        getIsUrl();
    QString     getCurPlayUrl();
    MEDIAINFO   getCurPlayMedia();

    void    startGetPoint();
    void    stopGetPoint();

    void    analyDeviceLastChange(char* change);
    void    analyCtrlActionComplete(IXML_Document* xmlDoc,char* url);
    QString getValForXML(QDomNode node, QString name);
    void    analyDeviceSupInfo(QString info,QString url);

    int     CtlvolumeChange(int volume);       // 音量改变
    int     CtrlSetMute(int mute);           // 设置静音
    int     positionChange(int proVol);     // 播放进度改变
    int     playFilePath(QString filePath); // 播放文件
    int     playUrlPath(QString url);       // 播放url
    void    playClick();
    int     paushFile();
    int     stopFile();
    void    sendGetProtocolInfo(const char *udn);

    void addDevice(deviceInfo device);
    void delDevice(char* udn,bool isVerify);
    bool getDevSelUdnFlag();
    QList<deviceInfo> getDeviceList();
    char* getDeviceSink();
    bool    checkSendMediaExt(MEDIAINFO media);
    bool    checkSendPathExt(QString path);
private:

private:
    QString createUUid();
    QString getVideoSuffix(QString httpType);
    QString getAudioSuffix(QString httpType);

signals:
    void sendCtrlportState(QString state);
    void sendCtrlVolumeChange(int volume);
    void sendCtrlMuteChange(bool mute);
    void sendGetDeviceSup(deviceInfo dev);

    void sendUPDevList(QList<deviceInfo>,bool isAdd);
    void sendUpPoint(int RelTime,int duration);
    void sendUPNPCon(bool isconnect);

public slots:
private slots:
    void pointTimeOut();
    void stopTimeOut();
    void playTimeOut();
private:
    QTimer*     _pointTimer;
    QTimer*     _stopTimer;
    QTimer*     _ctrlPlayTimer;

    QList<deviceInfo>  _deviceList;

    QString     _currentUrl;
    MEDIAINFO   _currentMedia;

    QString     _curFilePath;

    QString     _curPlayPoint;
    bool        _selUdnDel;

    QString     _selDevUDN;
    QString     _curDevUDN;

    QString     _curState;
    QString     _localIp;
    bool        _curMute;
    bool        _clickUnCon;
    bool        _lockStop;  // 在播放文件时，先发送了stop过去，防止在播放时设备返回发送的stop信号，导致播放失败
    bool        _playSource;// 根据进度时间来判断是否播放结束，播放结束则忽略发送的stop信息

    //deviceSupInfo _deviceSups;  // 连接设备支持的媒体类型

    // content_type 对照表
    // http://tools.jb51.net/table/http_content_type/
    // http://tool.oschina.net/commons/
    QHash<QString, QString> _contentVideoType;
    QHash<QString, QString> _contentAudioType;

public:
    QList<AvDevice> devInfoList;

    // 只关注这3种服务
    const char *AvServiceType[SERVICE_SERVCOUNT] = {
        "urn:schemas-upnp-org:service:RenderingControl:1",
        "urn:schemas-upnp-org:service:AVTransport:1",
        "urn:schemas-upnp-org:service:ConnectionManager:1"
    };
};

#endif // CONTROLMANAGER_H
