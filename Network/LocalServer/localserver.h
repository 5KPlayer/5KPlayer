#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <QObject>

class QLocalServer;

class LocalServer : public QObject
{
    Q_OBJECT
public:
    explicit LocalServer(QObject *parent = 0);

    bool connectToServer(const char *msg);
    void createServer();

private slots:
    void newConnection();
    void readyRead();
    void disConnected();

Q_SIGNALS:
    void receiveMsg(QString msg);

private:
    QLocalServer *_server;
};

#endif // LOCALSERVER_H
