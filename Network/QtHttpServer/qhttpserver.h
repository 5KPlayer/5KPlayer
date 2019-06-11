#ifndef QHTTPSERVER_H
#define QHTTPSERVER_H

#include <QObject>
#include <QtNetwork/QHostAddress>


class QHttpRequest;
class QHttpReply;

class QHttpServer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int maxPendingConnections READ maxPendingConnections WRITE setMaxPendingConnections NOTIFY maxPendingConnectionsChanged)
public:
    explicit QHttpServer(QObject *parent = 0);

    bool listen(const QHostAddress &address = QHostAddress::Any, quint16 port = 0);
    void close();

    bool isListening() const;

    void setMaxPendingConnections(int maxPendingConnections);
    int maxPendingConnections() const;

    quint16 serverPort() const;
    QHostAddress serverAddress() const;

    QAbstractSocket::SocketError serverError() const;
    QString errorString() const;

Q_SIGNALS:
    void maxPendingConnectionsChanged(int maxPendingConnections);
    void incomingConnection(QHttpRequest *request, QHttpReply *reply);

private:
    class Private;
    Private *d;
    Q_DISABLE_COPY(QHttpServer)
};

#endif // QHTTPSERVER_H
