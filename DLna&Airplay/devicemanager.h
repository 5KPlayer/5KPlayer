#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include "singleton.h"
#include "devstruct.h"
#include "ithread.h"

#define Device DeviceManager::getInstance()
class DeviceManager : public QObject,public Singleton<DeviceManager>
{
    Q_OBJECT
    friend class Singleton<DeviceManager>;
    friend class QSharedPointer<DeviceManager>;
public:
    explicit DeviceManager(QObject *parent = 0);

    void    setDevState(QString state);
    QString getDevState();

    void    setTransing(QString url);
    char*   createEventLastChange(int type, char *actionName, QList<actionKey_Value> args);

    void    deviceSeek(QString time);
    void    deviceTransportState(QString state);
    void    deviceMute();

    void    deviceVolumeChange(int volume);
    void    devicePlayChange();
    void    devicePaushChange();
    void    deviceTransIngChange();
    void    deviceStopChange();
    void    deviceNo_MediaChange();
    void    deviceMuteChange(bool mute);
    void    deviceUnCon();  //

    QStringList getM3uPaths(QString m3u);
    QString     getNextM3u();
    bool        getIsM3u();
signals:
    void    deviceTransState(QString state,QString url);
    void    deviceSeekPoint(int relTime);
public slots:
private:
    QString _dev_state;
    QString _trans_URL;
    QStringList _trans_Urls;
    bool        _isM3u;
    int         _seekPos;

};

#endif // DEVICEMANAGER_H
