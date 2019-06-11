#include "qhttpserver.h"
#include <QtNetwork/QTcpServer>
#include "qhttpconnection.h"

class QHttpServer::Private : public QTcpServer
{
    Q_OBJECT
public:
    Private(QHttpServer *parent);

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    QHttpServer *q;
};

QHttpServer::Private::Private(QHttpServer *parent)
    : QTcpServer(parent), q(parent)
{
    setMaxPendingConnections(1000);
}

void QHttpServer::Private::incomingConnection(qintptr socketDescriptor)
{
    QHttpConnection *connection = new QHttpConnection(socketDescriptor, this);
    connect(connection, SIGNAL(ready(QHttpRequest *, QHttpReply *)), q, SIGNAL(incomingConnection(QHttpRequest *, QHttpReply *)));
}

QHttpServer::QHttpServer(QObject *parent) : QObject(parent), d(new Private(this))
{

}

bool QHttpServer::listen(const QHostAddress &address, quint16 port)
{
    return d->listen(address, port);
}

void QHttpServer::close()
{
    d->close();
}

bool QHttpServer::isListening() const
{
    return d->isListening();
}

void QHttpServer::setMaxPendingConnections(int maxPendingConnections)
{
    if (d->maxPendingConnections() == maxPendingConnections) return;
    d->setMaxPendingConnections(maxPendingConnections);
    emit maxPendingConnectionsChanged(maxPendingConnections);
}

int QHttpServer::maxPendingConnections() const
{
    return d->maxPendingConnections();
}

quint16 QHttpServer::serverPort() const
{
    return d->serverPort();
}

QHostAddress QHttpServer::serverAddress() const
{
    return d->serverAddress();
}

QAbstractSocket::SocketError QHttpServer::serverError() const
{
    return d->serverError();
}

QString QHttpServer::errorString() const
{
    return d->errorString();
}

#include "qhttpserver.moc"
