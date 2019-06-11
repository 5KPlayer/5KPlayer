#ifndef QHTTPCONNECTION_H
#define QHTTPCONNECTION_H

#include <QtNetwork/QTcpSocket>

class QHttpRequest;
class QHttpReply;

class QHttpConnection : public QTcpSocket
{
    Q_OBJECT
public:
    explicit QHttpConnection(qintptr socketDescriptor, QObject *parent = 0);
    ~QHttpConnection();

    const QHttpRequest *requestFor(QHttpReply *reply);

signals:
    void ready(QHttpRequest *request, QHttpReply *reply);

private:
    class Private;
    Private *d;
    Q_DISABLE_COPY(QHttpConnection)
};

#endif // QHTTPCONNECTION_H
