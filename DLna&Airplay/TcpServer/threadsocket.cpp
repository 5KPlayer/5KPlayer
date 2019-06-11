#include "threadsocket.h"
#include "z_httpserver.h"
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QtDebug>
#include "qhttpparser.h"

qint64 i_bufferSize = 512 * 1024; //1B=8byte 1024B=1KB 1024KB=1MB

ThreadSocket::ThreadSocket(qintptr handle, QObject *parent) : QThread(parent)
{
    m_handle = handle;
}

ThreadSocket::~ThreadSocket()
{
    if(this->isRunning())
    {
        this->quit();
        this->wait();
    }
}

bool ThreadSocket::setStreamFile(const QString &file)
{
    if(m_file && !file.isEmpty() && m_file->fileName() == file)
        return true;

    closeFile();

    QFileInfo finfo(file);
    if(!finfo.exists())
    {
        qWarning() << "不存在 >> " << file;
        return false;
    }

    m_contentType = getContentType(finfo.suffix().toLower());

    m_file = new QFile(file);
    if(!m_file->open(QIODevice::ReadOnly))
    {
        qWarning() << "文件读取失败 >> " << file;
        closeFile();
        return false;
    }
    return true;
}

void ThreadSocket::closeFile()
{
    if(m_file)
    {
        if(m_file->isOpen())
            m_file->close();

        delete m_file;
        m_file = nullptr;
    }
}

void ThreadSocket::closeSocket()
{
    if(m_socket)
    {
        m_socket->disconnect();
        m_socket->disconnectFromHost();
        m_socket->close();
        delete m_socket;
        m_socket = nullptr;
    }
}

void ThreadSocket::run()
{
    closeSocket();

    m_socket = new QTcpSocket;

    connect(m_socket, &QTcpSocket::bytesWritten, this, &ThreadSocket::myBytesWritten, Qt::DirectConnection);
    connect(m_socket, &QTcpSocket::readyRead,    this, &ThreadSocket::myReadData,     Qt::DirectConnection);
    connect(m_socket, &QTcpSocket::disconnected, this, &ThreadSocket::quit,           Qt::DirectConnection);

    m_socket->setSocketDescriptor(m_handle);

    this->exec();

    closeSocket();
    closeFile();
}

void ThreadSocket::myReadData()
{
    if(!m_socket) return;

    QString str;
    {
        m_recvData += m_socket->readAll();

        //qDebug() << "\nrecv:" << m_recvData ;

        if(!m_recvData.endsWith("\r\n\r\n"))
        {
            return;
        }

        //qDebug() << "---" << __func__ << m_recvData;

        //解析请求
        QHttpParser parser;
        bool b_finish = parser.parserRequest(m_recvData);
        m_recvData.clear();
        if(!b_finish) //解析失败
        {
            quit();
            return;
        }
        if(parser.method().toLower() != "get")
        {
            quit();
            return;
        }
        QString url = parser.url();
        if(!url.startsWith('/'))
        {
            quit();
            return;
        }
        QStringList args = url.split('/', QString::SkipEmptyParts); // /xxxx/xx.mp4
        if(args.size() != 2)
        {
            quit();
            return;
        }
        url = args[0];

        //查找文件
        Z_HttpServer *server = qobject_cast<Z_HttpServer *>(this->parent());
        const QString file = server->file(url);

        //设置文件流
        if(!setStreamFile(file))
        {
            quit();
            return;
        }

        str = parser.headerValue("Range");
    }
    if(str.isEmpty())
    {
        writeFileData200(0);
        return;
    }

    //Range: bytes=0-1024
    //Range: bytes=0-    整个文件
    //Range: bytes=-1024 最后1024个字节
    QStringList list = str.split("=");

    if(list.count() == 2)
    {
        str = list[1];
        str.replace(" ", "");

        qint64 startPos, endPos;

        list.clear();
        list = str.split("-", QString::SkipEmptyParts);

        const qint64 fileSize = m_file->size();

        if(str.startsWith('-'))    // -x
        {
            endPos   = fileSize - 1;
            startPos = endPos - list.at(0).toInt();
        }
        else if(str.endsWith('-')) // x-
        {
            startPos = list.at(0).toInt();
            endPos   = fileSize - 1;
        }
        else // x-x
        {
            startPos = list.at(0).toInt();
            endPos   = list.at(1).toInt();
        }
        //qDebug() << "--writeFileData206:" << startPos << endPos;
        writeFileData206(startPos, endPos);
    }
    else
    {
        quit();
    }
}

void ThreadSocket::myBytesWritten(const qint64 &bytes)
{
    i_writBytesCount -= bytes;
    if(m_socket && i_writBytesCount > 0)
    {
        m_socket->write(m_file->read(i_bufferSize)); //1KB = 1024字节
    }
}

void ThreadSocket::writeFileData200(const qint64 &startPos)
{
    if(!m_file->isOpen())
        return;
    const qint64 fileSize = m_file->size();
    if(startPos < fileSize)
    {
        const qint64 endPos = fileSize - 1;
        const qint64 bufferSize = fileSize - startPos;

        const QByteArray head = QString("HTTP/1.1 200 OK\r\n"
                                        "Server: 5KPlayer-Digiarty\r\n"
                                        "Content-Disposition: attachment;filename=%1\r\n"
                                        "Cache-control: no-cache\r\n"
                                        "Pragma: no-cache\r\n"
                                        "Connection: close\r\n"
                                        "Accept-Ranges: bytes\r\n"
                                        "Content-Length: %2\r\n"
                                        "Content-Type: %3\r\n"
                                        "Content-Range: bytes %4-%5/%6\r\n"
                                        "Access-Control-Allow-Origin: *\r\n"
                                        "\r\n")
                .arg(m_file->fileName())
                .arg(bufferSize)
                .arg(m_contentType)
                .arg(startPos)
                .arg(endPos)
                .arg(fileSize)
                .toUtf8();

        m_file->seek(startPos);
        i_writBytesCount = head.size() + (fileSize - startPos);
        m_socket->write(head);
    }
}

void ThreadSocket::writeFileData206(const qint64 &startPos, const qint64 &endPos)
{
    if(!m_file->isOpen())
        return;
    const qint64 fileSize = m_file->size();
    if(startPos < fileSize && startPos < endPos)
    {
        //const qint64 endPos = fileSize - 1;
        const qint64 bufferSize = endPos - startPos + 1;

        const QByteArray head = QString("HTTP/1.1 206 Partial Content\r\n"
                                        "Server: 5KPlayer-Digiarty\r\n"
                                        "Content-Disposition: attachment;filename=%1\r\n"
                                        "Cache-control: no-cache\r\n"
                                        "Pragma: no-cache\r\n"
                                        "Content-Length: %2\r\n"
                                        "Content-Type: %3\r\n"
                                        "Connection: close\r\n"
                                        "Accept-Ranges: bytes\r\n"
                                        "Content-Range: bytes %4-%5/%6\r\n"
                                        "Access-Control-Allow-Origin: *\r\n"
                                        "\r\n")
                .arg(m_file->fileName())
                .arg(bufferSize)
                .arg(m_contentType)
                .arg(startPos)
                .arg(endPos)
                .arg(fileSize)
                .toUtf8();

        m_file->seek(startPos);
        i_writBytesCount = head.size() + (fileSize - startPos);
        m_socket->write(head);
    }
}

QString ThreadSocket::getContentType(const QString &suffix)
{
    if(m_mimeType.isEmpty())
    {
        //video
        m_mimeType["asf"]   = "video/x-ms-asf";
        m_mimeType["asx"]   = "video/x-ms-asf";
        m_mimeType["avi"]   = "video/avi";
        m_mimeType["ivf"]   = "video/x-ivf";
        m_mimeType["m1v"]   = "video/x-mpeg";
        m_mimeType["m2v"]   = "video/x-mpeg";
        m_mimeType["m4e"]   = "video/mpeg4";
        m_mimeType["movie"] = "video/x-sgi-movie";
        m_mimeType["mp2v"]  = "video/mpeg";
        m_mimeType["mp4"]   = "video/mpeg4";
        m_mimeType["mpa"]   = "video/x-mpg";
        m_mimeType["mpe"]   = "video/x-mpeg";
        m_mimeType["mpeg"]  = "video/mpg";
        m_mimeType["mpg"]   = "video/mpg";
        m_mimeType["mps"]   = "video/x-mpeg";
        m_mimeType["mpv"]   = "video/mpg";
        m_mimeType["mpv2"]  = "video/mpeg";
        m_mimeType["rv"]    = "video/vnd.rn-realvideo";
        m_mimeType["wm"]    = "video/x-ms-wm";
        m_mimeType["wmv"]   = "video/x-ms-wmv";
        m_mimeType["wmx"]   = "video/x-ms-wmx";
        m_mimeType["wvx"]   = "video/x-ms-wvx";

        //audio
        m_mimeType["acp"]   = "audio/x-mei-aac";
        m_mimeType["aif"]   = "audio/aiff";
        m_mimeType["aiff"]  = "audio/aiff";
        m_mimeType["aifc"]  = "audio/aiff";
        m_mimeType["au"]    = "audio/basic";
        m_mimeType["la1"]   = "audio/x-liquid-file";
        m_mimeType["lavs"]  = "audio/x-liquid-secure";
        m_mimeType["lmsff"] = "audio/x-la-lms";
        m_mimeType["m3u"]   = "audio/mpegurl";
        m_mimeType["midi"]  = "audio/mid";
        m_mimeType["mid"]   = "audio/mid";
        m_mimeType["mp2"]   = "audio/mp2";
        m_mimeType["mp3"]   = "audio/mp3";
        m_mimeType["mp4"]   = "audio/mp4";
        m_mimeType["mnd"]   = "audio/x-musicnet-download";
        m_mimeType["mp1"]   = "audio/mp1";
        m_mimeType["mns"]   = "audio/x-musicnet-stream";
        m_mimeType["mpga"]  = "audio/rn-mpeg";
        m_mimeType["pls"]   = "audio/scpls";
        m_mimeType["ra"]    = "audio/vnd.rn-realaudio";
        m_mimeType["ram"]   = "audio/x-pn-realaudio";
        m_mimeType["rmi"]   = "audio/mid";
        m_mimeType["rmm"]   = "audio/x-pn-realaudio";
        m_mimeType["rpm"]   = "audio/x-pn-realaudio-plugin";
        m_mimeType["snd"]   = "audio/basic";
        m_mimeType["wav"]   = "audio/wav";
        m_mimeType["wax"]   = "audio/x-ms-wax";
        m_mimeType["wma"]   = "audio/x-ms-wma";
        m_mimeType["xpl"]   = "audio/scpls";

        // image
        m_mimeType["fax"]   = "image/fax";
        m_mimeType["gif"]   = "image/gif";
        m_mimeType["ico"]   = "image/x-ico";
        m_mimeType["jfif"]  = "image/jpeg";
        m_mimeType["jpe"]   = "image/jpeg";
        m_mimeType["jpeg"]  = "image/jpeg";
        m_mimeType["jpg"]   = "image/jpeg";
        m_mimeType["net"]   = "image/pnetvue";
        m_mimeType["png"]   = "image/png";
        m_mimeType["rp"]    = "image/vnd.rn-realpix";
        m_mimeType["tif"]   = "image/tiff";
        m_mimeType["tiff"]  = "image/tiff";
        m_mimeType["wbmp"]  = "image/vnd.wap.wbmp";
    }
    return m_mimeType.value(suffix, "Unknown");
}
