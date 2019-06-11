#include "localserver.h"

#include <QLocalServer>
#include <QLocalSocket>

#define LOCAL_SERVER_NAME "5KPlayer"

LocalServer::LocalServer(QObject *parent) : QObject(parent)
{
    _server = NULL;
}

bool LocalServer::connectToServer(const char *msg)
{
    QSharedPointer<QLocalSocket> socket = QSharedPointer<QLocalSocket>(new QLocalSocket);
    socket->connectToServer(LOCAL_SERVER_NAME);
    if(socket->waitForConnected())
    {
        const uint len = qstrlen(msg);
        if(len == 0) {
            socket->write("-show");
            socket->waitForBytesWritten();
            socket->flush();
        }
        else
        {
            socket->write(msg);
            socket->waitForBytesWritten();
            socket->flush();
        }
        socket->disconnectFromServer();
        socket->close();
        return true;
    }
    return false;
}

void LocalServer::createServer()
{
    if(_server == NULL) {
        _server = new QLocalServer(this);
        connect(_server, SIGNAL(newConnection()), SLOT(newConnection()));
    }
    _server->listen(LOCAL_SERVER_NAME);
}

void LocalServer::newConnection()
{
    if(_server == NULL) return;
    QLocalSocket *newsocket = _server->nextPendingConnection();
    if(newsocket == NULL) return;
    //connect(newsocket, SIGNAL(readyRead()), SLOT(readyRead()));
    connect(newsocket, SIGNAL(disconnected()), SLOT(disConnected()));
}

void LocalServer::readyRead()
{
    QLocalSocket *socket = qobject_cast<QLocalSocket *>(this->sender());
    if(socket == NULL) return;

    Q_EMIT(receiveMsg(QString::fromUtf8(socket->readAll())));
}

void LocalServer::disConnected()
{
    QLocalSocket *socket = qobject_cast<QLocalSocket *>(this->sender());
    if(socket == NULL) return;

    QString msg = QString::fromUtf8(socket->readAll());
    if(!msg.isEmpty())
        Q_EMIT(receiveMsg(msg));

    socket->disconnectFromServer();
    socket->close();
    socket->deleteLater();
}
