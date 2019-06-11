#ifndef Z_HTTPSERVER_H
#define Z_HTTPSERVER_H

#include <QTcpServer>
#include "singleton.h"

class ThreadSocket;

#define ZHttp  Z_HttpServer::getInstance()

class Z_HttpServer : public QTcpServer, public Singleton<Z_HttpServer>
{
    Q_OBJECT
    friend class Singleton<Z_HttpServer>;
    friend class QSharedPointer<Z_HttpServer>;

public:
    //添加文件  返回hash值
    QString addFile(const QString &file);
    void    removeFile(const QString &file);

    QString start(const QString &ip = "");
    void stop();
    QString getHttp() const;

    //根据hash值找到文件
    QString file(const QString &hash) const;

protected:
    virtual void incomingConnection(qintptr handle);

private:
    Q_DISABLE_COPY(Z_HttpServer)
    explicit Z_HttpServer(QObject *parent = 0);
    ~Z_HttpServer();

    QHash<QString, QString> m_files;
    QList<ThreadSocket *> m_sockets;
};

#endif // Z_HTTPSERVER_H
