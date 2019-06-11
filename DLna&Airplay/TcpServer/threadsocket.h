#ifndef THREADSOCKET_H
#define THREADSOCKET_H

#include <QHash>
#include <QThread>
class QTcpSocket;
class QFile;

class ThreadSocket : public QThread
{
    Q_OBJECT
public:
    explicit ThreadSocket(qintptr handle, QObject *parent = 0);
    virtual ~ThreadSocket();

protected:
    virtual void run();

private:
    void myReadData();
    void myBytesWritten(const qint64 &);

    //写文件流
    void writeFileData200(const qint64 &startPos); // 200 正常返回
    void writeFileData206(const qint64 &startPos, const qint64 &endPos); //206 断点续传

    QString getContentType(const QString &suffix); //根据后缀名 获取ContentType

    bool setStreamFile(const QString &file); //设置流文件

    void closeFile();  //关闭文件
    void closeSocket();//关闭socket

private:
    qintptr     m_handle;//socket 描述
    QTcpSocket *m_socket         = nullptr;
    QFile      *m_file           = nullptr;
    QString     m_contentType    = "";
    qint64      i_writBytesCount = -1;
    QByteArray  m_recvData       = "";

    // content_type 对照表
    // http://tools.jb51.net/table/http_content_type/
    // http://tool.oschina.net/commons/
    QHash<QString, QString> m_mimeType;
};

#endif // THREADSOCKET_H
