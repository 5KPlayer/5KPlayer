#ifndef HTTPSERVERHANDLE_H
#define HTTPSERVERHANDLE_H

#include <QObject>

class QFile;
class QHttpServer;
class QHttpRequest;
class QHttpReply;

class HttpServerHandle : public QObject
{
    Q_OBJECT
public:
    explicit HttpServerHandle(QObject *parent = 0);
    ~HttpServerHandle();

    /*!
     * \brief setCurFile
     * \param file 请求的当前文件
     */
    void setCurFile(const QString &file);

    /*!
     * \brief listen 开启监听
     * \param port: server监听的端口号
     */
    bool listen(quint16 port);

    void close();

private slots:
    void incomingConnection(QHttpRequest *, QHttpReply *);

private:
    QFile *_file;
    QHttpServer *_server;
};

#endif // HTTPSERVERHANDLE_H
