#include "z_httpserver.h"
#include "threadsocket.h"

#include <QCryptographicHash>
#include <QHostAddress>
#include <QTcpSocket>
#include <QFileInfo>

Z_HttpServer::Z_HttpServer(QObject *parent) : QTcpServer(parent)
{
    this->setMaxPendingConnections(4);
}

Z_HttpServer::~Z_HttpServer()
{
    stop();
}

QString Z_HttpServer::addFile(const QString &file)
{
    QFileInfo info(file);
    QByteArray hash = QCryptographicHash::hash(file.toUtf8(), QCryptographicHash::Md5)
                      .toHex().toLower();
    if(!m_files.contains(hash))
        m_files[hash] = file;
    return hash + "/" + info.fileName().toUtf8().toPercentEncoding();
}

void Z_HttpServer::removeFile(const QString &file)
{
    QString hash = QCryptographicHash::hash(file.toUtf8(), QCryptographicHash::Md5)
            .toHex().toLower();
    if(m_files.contains(hash))
        m_files.remove(hash);
}

QString Z_HttpServer::file(const QString &hash) const
{
    return m_files.value(hash.toLower(), "");
}

QString Z_HttpServer::start(const QString &ip)
{
    if(!this->isListening())
    {
        QHostAddress addr(ip);
        if(ip.isEmpty())
            addr = QHostAddress(QHostAddress::Any);

        for(quint16 port = 12345; port < 65535; ++port)
        {
            if(this->listen(addr, port))
            {
                qDebug() << "\n---" << __func__ << ip << port;
                break;
            }
        }
    }
    return getHttp();
}

void Z_HttpServer::stop()
{
    this->disconnect();
    this->close();

    foreach (ThreadSocket *socket, m_sockets) {
        delete socket;
    }
    m_sockets.clear();
}

QString Z_HttpServer::getHttp() const
{
    if(!this->isListening())
        return "";
    return QString("http://%1:%2")
            .arg(this->serverAddress().toString())
            .arg(this->serverPort());
}

void Z_HttpServer::incomingConnection(qintptr handle)
{
    if(m_sockets.count() < this->maxPendingConnections())
    {
        ThreadSocket *socket = new ThreadSocket(handle, this);
        connect(socket, &ThreadSocket::finished, [this, socket](){
            m_sockets.removeOne(socket);
            socket->deleteLater();
        });
        m_sockets <<  socket;

        socket->start();
    }
    else
    {
        QTcpSocket socket;
        socket.setSocketDescriptor(handle);
        socket.write("HTTP/1.1 200 OK\r\n\r\n"
                     "More than the maximum connection!");
        socket.waitForBytesWritten();
        socket.disconnectFromHost();
        socket.close();
    }
}
