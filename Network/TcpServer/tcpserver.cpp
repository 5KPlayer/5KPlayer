#include "tcpserver.h"
#include "tcpsocket.h"
#include <QHostInfo>
#include <QHostAddress>

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{
    this->setMaxPendingConnections(1);
}

TcpServer::~TcpServer()
{
    disConnect();
}

bool TcpServer::listenPort(uint port)
{
    if(!this->isListening())
        return this->listen(QHostAddress::Any, port);
    return true;
}

void TcpServer::setFileName(const QString &file)
{
    _fileName = file;
}

void TcpServer::disConnect()
{
    this->disconnect();
    this->close();
}

void TcpServer::incomingConnection(qintptr handle)
{
    TcpSocket *socket = new TcpSocket(_fileName);

    socket->setSocketDescriptor(handle);
}
