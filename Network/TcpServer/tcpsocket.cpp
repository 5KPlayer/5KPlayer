#include "tcpsocket.h"
#include <QFile>
#include <QtDebug>

TcpSocket::TcpSocket(const QString & file, QObject *parent) : QTcpSocket(parent)
{
    p_file = NULL;
    b_connected = true;
    i_bufferSize = 1024 * 10;//10KB

    setFileName(file);
    connect(this, SIGNAL(readyRead()),          this, SLOT(myReadData()));
    connect(this, SIGNAL(disconnected()),       this, SLOT(myDisConnected()));
    connect(this, SIGNAL(bytesWritten(qint64)), this, SLOT(myBytesWritten(qint64)));
}

TcpSocket::~TcpSocket()
{
    closeFile();
}

void TcpSocket::setBufferSize(const qint64 &size)
{
    i_bufferSize = size;
}

void TcpSocket::setFileName(const QString &fileName)
{
    closeFile();

    if(!QFile::exists(fileName))
        return;

    p_file = new QFile(fileName);
    if(!p_file->open(QIODevice::ReadOnly)) {
        closeFile();
        return;
    }
    _fileName = fileName;
}

void TcpSocket::myReadData()
{
    QByteArray array = this->readAll();
    const int t_pos = array.indexOf("Range: bytes=");
    if(t_pos == -1)
        return;
    const int endPos = array.indexOf("\r\n", t_pos+13);
    if(endPos == -1)
        return;

    // Range: bytes=xxx- //只针对vlc
    QByteArray tmpArray = array.mid(t_pos+13, endPos-(t_pos+13)-1);
    const qint64 startPos = tmpArray.toLongLong();//xxx-

    writeFileData(startPos);
}

void TcpSocket::writeFileData(const qint64 &startPos)
{
    if(!p_file->isOpen())
        return;
    const qint64 fileSize = p_file->size();
    if(startPos < fileSize)
    {
        const qint64 endPos = fileSize - 1;
        const qint64 bufferSize = fileSize - startPos;

        const QByteArray head = QString("HTTP/1.1 200 OK\r\n"
                                     "Server: 5KPlayer-Digiarty\r\n"
                                     "Content-Disposition: attachment;filename=%1\r\n"
                                     "Cache-control: no-cache\r\n"
                                     "Pragma: no-cache\r\n"
                                     "Content-Type: video/mp4\r\n"
                                     "Content-Length: %2\r\n"
                                     "Connection: close\r\n"
                                     "Accept-Ranges: bytes\r\n"
                                     "Content-Range: bytes %3-%4/%5\r\n"
                                     "Access-Control-Allow-Origin: *\r\n"
                                     "\r\n")
                .arg(p_file->fileName())
                .arg(bufferSize)
                .arg(startPos)
                .arg(endPos)
                .arg(fileSize)
                .toUtf8();

        p_file->seek(startPos);
        i_writBytesCount = head.size() + (fileSize - startPos);
        this->write(head);
    }
}

void TcpSocket::myBytesWritten(const qint64 &bytes)
{
    i_writBytesCount -= bytes;
    if(b_connected && i_writBytesCount > 0)
    {
        this->write(p_file->read(i_bufferSize));//1KB = 1024字节
    }
    else
    {
        myDisConnected();
    }
}

void TcpSocket::myDisConnected()
{
    if(!b_connected)
        return;

    b_connected = false;
    this->disconnect();//断开所有信号
    this->disconnectFromHost();
    this->close();
    this->deleteLater();
}

void TcpSocket::closeFile()
{
    if(p_file == NULL)
        return;

    if(p_file->isOpen())
        p_file->close();

    delete p_file;
    p_file = NULL;
}
