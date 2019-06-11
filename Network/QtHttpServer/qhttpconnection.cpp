#include "qhttpconnection.h"
#include "qhttprequest.h"
#include "qhttpreply.h"

#include <QtCore/QTime>
#include <QtCore/QUrl>

class QHttpConnection::Private : public QObject
{
    Q_OBJECT
public:
    Private(qintptr socketDescriptor, QHttpConnection *parent);

private slots:
    void upgrade(const QByteArray &to, const QUrl &url, const QHash<QByteArray, QByteArray> &rawHeaders);
    void requestReady();
    void replyDone(QObject *);

private:
    QHttpConnection *q;
    int keepAlive;

public:
    QMap<QObject*, QHttpRequest*> requestMap;
    QTime timer;
};


QHttpConnection::Private::Private(qintptr socketDescriptor, QHttpConnection *parent)
    : QObject(parent)
    , q(parent)
    , keepAlive(100)
{
    q->setSocketOption(KeepAliveOption, 1);
    q->setSocketDescriptor(socketDescriptor);

    QHttpRequest *request = new QHttpRequest(q);
    connect(request, SIGNAL(ready()), this, SLOT(requestReady()));
    connect(request, SIGNAL(upgrade(QByteArray, QUrl, QHash<QByteArray, QByteArray>)), this, SLOT(upgrade(QByteArray, QUrl, QHash<QByteArray, QByteArray>)));

    timer.start();
    connect(q, SIGNAL(disconnected()), q, SLOT(deleteLater()));
}

void QHttpConnection::Private::upgrade(const QByteArray &to, const QUrl &url, const QHash<QByteArray, QByteArray> &rawHeaders)
{
    QHttpRequest *request = qobject_cast<QHttpRequest *>(sender());
    disconnect(request, 0, this, 0);
    request->deleteLater();
}

void QHttpConnection::Private::requestReady()
{
    QHttpRequest *request = qobject_cast<QHttpRequest *>(sender());
    disconnect(request, SIGNAL(ready()), this, SLOT(requestReady()));
    QHttpReply *reply = new QHttpReply(q);
    connect(reply, SIGNAL(destroyed(QObject *)), this, SLOT(replyDone(QObject*)));
    requestMap.insert(reply, request);
    emit q->ready(request, reply);

    if (request->hasRawHeader("Connection")) {
        if (request->rawHeader("Connection") == QByteArray("Keep-Alive").toLower()) {
            if (keepAlive > 0) {
                reply->setRawHeader("Keep-Alive", QString::fromUtf8("timeout=1, max=%1").arg(keepAlive--).toUtf8());
                reply->setRawHeader("Connection", "Keep-Alive");
                request = new QHttpRequest(q);
                connect(request, SIGNAL(ready()), this, SLOT(requestReady()));
            } else {
                reply->setRawHeader("Connection", "Close");
                keepAlive = 0;
            }
        } else {
            reply->setRawHeader("Connection", "Close");
            keepAlive = 0;
        }
    } else {
        reply->setRawHeader("Connection", "Close");
        keepAlive = 0;
    }
}

void QHttpConnection::Private::replyDone(QObject *reply)
{
    if (requestMap.contains(reply)) {
        QHttpRequest *request = requestMap.take(reply);
        request->deleteLater();
    }
    if (keepAlive == 0 && requestMap.isEmpty()) {
        q->disconnectFromHost();
    }
}

QHttpConnection::QHttpConnection(qintptr socketDescriptor, QObject *parent)
    : QTcpSocket(parent)
    , d(new Private(socketDescriptor, this))
{
}

QHttpConnection::~QHttpConnection()
{
}

const QHttpRequest *QHttpConnection::requestFor(QHttpReply *reply)
{
    return d->requestMap.value(reply);
}

#include "qhttpconnection.moc"
