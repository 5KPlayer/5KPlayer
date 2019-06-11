#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QTcpSocket>

class QFile;

class TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpSocket(const QString &, QObject *parent = 0);
    ~TcpSocket();

    void setBufferSize(const qint64 &);
    void setFileName(const QString &);

private slots:
    void myReadData();
    void myDisConnected();
    void myBytesWritten(const qint64 &);

private:
    void closeFile();
    void writeFileData(const qint64 &);

private:
    QFile *p_file;
    QString _fileName;
    qint64 i_bufferSize;
    qint64 i_writBytesCount;

    bool b_connected;
};

#endif // TCPSOCKET_H
