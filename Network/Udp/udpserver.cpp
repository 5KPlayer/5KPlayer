#include "udpserver.h"

#include <QUdpSocket>
#include <QHostInfo>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QtDebug>
#include "librarydata.h"
#include "playermainwidget.h"

UdpServer::UdpServer(QObject *parent) : QObject(parent)
{
    _udpSocket = new QUdpSocket(this);
    _port = 10000;
    while (!_udpSocket->bind(_port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)){
        _port++;
    }
    getlocalIp();
    connect(_udpSocket, SIGNAL(readyRead()), SLOT(processPendingDatagrams()));

    this->sendMessage(UDPJOIN + QHostInfo::localHostName());
    connect(this,SIGNAL(udpClientJion(NetInfo)),LibData,SLOT(addNetList(NetInfo)));
    connect(this,SIGNAL(udpClientLeavl(QString)),LibData,SLOT(removeNet(QString)));
}

UdpServer::~UdpServer()
{
    this->sendMessage(UDPLEAVE);
    sendMsgToSerCPT("end");
    sendMsgToSerCPT("disconnected");
    _udpSocket->disconnect();
    _udpSocket->close();
}

void UdpServer::sendMessage(const QString &str)
{
    _udpSocket->writeDatagram(str.toLocal8Bit(), QHostAddress::Broadcast, _port);
}

void UdpServer::sendMsgToConCPT(const QString &str)
{
    Q_UNUSED(str);
    if(LibData->b_isConCPT) {
//        NetInfo udpInfo = LibData->getUdpClient();
//        QString IP = udpInfo.ipAddr;
//        qint16 prot = udpInfo.port;
//        _udpSocket->writeDatagram(str.toLocal8Bit(), QHostAddress(IP), prot);
    }
}

void UdpServer::sendMsgToSerCPT(const QString &str)
{
    if(LibData->b_isCPTCon) {
        NetInfo udpInfo = LibData->getUdpServer();
        QString IP = udpInfo.ipAddr;
        qint16 prot = udpInfo.port;
        _udpSocket->writeDatagram(str.toLocal8Bit(), QHostAddress(IP), prot);
        _udpSocket->flush();
    }
}

void UdpServer::processPendingDatagrams()
{
    while(_udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        QHostAddress sender;
        quint16 senderPort;

        datagram.resize(_udpSocket->pendingDatagramSize());
        _udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        const QString sendIp = QHostAddress(sender.toIPv4Address()).toString();

        // 获取消息头
        QString datagramHead = datagram.left(2);
        QString ComputerName;
        //分析读取的信息
        if(datagramHead == UDPLEAVE) {
            if(!checkIsLocalIp(sendIp)) {
                if(LibData->b_isConCPT) {
                    sendMsgToConCPT("end");
                    LibData->airDisConnect();
                } else {
                    sendMsgToSerCPT("end");
                    sendMsgToSerCPT("disconnected");
                    LibData->clearUdpServer();
                }
            }
            emit udpClientLeavl(sendIp);
        } else if(datagramHead == UDPJOIN) {
            ComputerName = datagram.mid(2);
            if(!checkIsLocalIp(sendIp)) {
                NetInfo netInfo;
                netInfo.ipAddr = sendIp;
                netInfo.machineName = ComputerName;
                netInfo.port = senderPort;
                emit udpClientJion(netInfo);
                this->sendMessage(UDPRESPOND + QHostInfo::localHostName());
            }
        } else if(datagramHead == UDPRESPOND) {
            ComputerName = datagram.mid(2);
            if(!checkIsLocalIp(sendIp)) {
                NetInfo resNewInfo;
                resNewInfo.ipAddr       = sendIp;
                resNewInfo.machineName  = ComputerName;
                resNewInfo.port         = senderPort;
                emit udpClientJion(resNewInfo);
            }
        } else if(datagramHead == UDPMESSAGE) {
            ;
        } else if(datagramHead == UDPCONNECT) {
            if(!checkIsLocalIp(sendIp)) {
                NetInfo netInfo;
                netInfo.ipAddr = sendIp;
                netInfo.machineName = datagram.mid(2);
                netInfo.port = senderPort;
                LibData->setUdpServer(netInfo);
                sendMsgToSerCPT("connected");
            }
        } else if(datagramHead == UDPDISCON) {
            emit controlSignal("end");
            if(LibData->b_isCPTCon) {
                sendMsgToSerCPT("disconnected");
                LibData->clearUdpServer();
            }
        } else {
            if(datagramHead == "bf")
            {
                PMW->playUrl("http://" + sendIp + datagram.mid(2));
                PMW->showOut();
            }
            else
            {
                emit controlSignal(datagram);
            }
        }
    }
}

void UdpServer::getlocalIp()
{
    _localIps.clear();
    QHostInfo info = QHostInfo::fromName(QHostInfo::localHostName());
    info.addresses();//QHostInfo的address函数获取本机ip地址
    //如果存在多条ip地址ipv4和ipv6：
    foreach(QHostAddress address,info.addresses())
    {
        if(address.protocol()==QAbstractSocket::IPv4Protocol){//只取ipv4协议的地址
            _localIps << address.toString();
        }
    }
}

bool UdpServer::checkIsLocalIp(QString Ip)
{
    bool ret = false;
    foreach (QString localIp, _localIps) {
        if(localIp == Ip) {
            ret = true;
            break;
        }
    }
    return ret;
}
