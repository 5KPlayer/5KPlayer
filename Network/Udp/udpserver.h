#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include "dystructu.h"
#include "singleton.h"

#define UDPLEAVE     "01"  // 退出消息头
#define UDPJOIN      "02"  // 加入消息头
#define UDPRESPOND   "03"  // 回应加入消息头

#define UDPMESSAGE   "04"  // 发送消息头
#define UDPCONNECT   "11"  // 连接的消息头
#define UDPDISCON    "12"  // 断开连接


class QUdpSocket;

#define UdpSev UdpServer::getInstance()
class UdpServer : public QObject,public Singleton<UdpServer>
{
    Q_OBJECT
    friend class  Singleton<UdpServer>;
    friend class QSharedPointer<UdpServer>;

public:
    //广播信息
    void sendMessage(const QString &str);

    //发送给连接的计算机 信息
    void sendMsgToConCPT(const QString &str);
    //发送给服务端的计算机信息
    void sendMsgToSerCPT(const QString &str);

signals:
    void udpClientJion(NetInfo);        // 加入
    void udpClientLeavl(QString);       // 退出
    void controlSignal(const QString &);// 控制信号 pause play end volume seek

private slots:
    //处理接受到的消息
    void processPendingDatagrams();

private:
    Q_DISABLE_COPY(UdpServer)
    explicit UdpServer(QObject *parent = 0);
    ~UdpServer();

    void getlocalIp();                  // 获取本地的Ip地址
    bool checkIsLocalIp(QString Ip);    // 检查当前Ip地址是否是本机IP地址 是返回true

    quint64 _port;
    QUdpSocket *_udpSocket;
    QStringList _localIps;       // 本机的IP
};

#endif // UDPSERVER_H
