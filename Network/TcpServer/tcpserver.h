#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);
    ~TcpServer();

    bool listenPort(uint port);
    void setFileName(const QString &);
    void disConnect();

protected:
    virtual void incomingConnection(qintptr handle);

private:
    QString _fileName;
};

#endif // TCPSERVER_H
