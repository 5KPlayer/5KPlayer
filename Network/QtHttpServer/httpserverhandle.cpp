#include "httpserverhandle.h"

#include "qhttpserver.h"
#include "qhttprequest.h"
#include "qhttpreply.h"
#include <QFile>
#include <QtDebug>

HttpServerHandle::HttpServerHandle(QObject *parent) : QObject(parent)
{
    _file = NULL;
    _server = NULL;
}

HttpServerHandle::~HttpServerHandle()
{
    close();
}

bool HttpServerHandle::listen(quint16 port)
{
    if(_server == NULL)
    {
        _server = new QHttpServer(this);
        connect(_server, SIGNAL(incomingConnection(QHttpRequest*,QHttpReply*)),
                           SLOT(incomingConnection(QHttpRequest*,QHttpReply*)));
    }

    if(!_server->isListening())
    {
        return _server->listen(QHostAddress::Any, port);
    }
    return true;
}

void HttpServerHandle::close()
{
    if(_server)
    {
        _server->disconnect();
        _server->close();
        delete _server;
        _server = NULL;
    }

    if(_file)
    {
        _file->close();
        delete _file;
        _file = NULL;
    }
}

void HttpServerHandle::setCurFile(const QString &fileName)
{
    if(_file)
    {
        _file->close();
        delete _file;
        _file = NULL;
    }
    _file = new QFile(fileName, this);
    _file->open(QIODevice::ReadOnly);
}

void HttpServerHandle::incomingConnection(QHttpRequest *req, QHttpReply *reply)
{
    if(req->hasRawHeader("Range"))
    {
        QString str = req->rawHeader("Range").replace("-","");
        const QString range = str.split("=",QString::SkipEmptyParts).at(1);

        const qint64 filesize = _file->size();

        const int i_range = filesize - range.toInt();

        reply->setStatus(206);
        reply->setRawHeader("Content-Length", QString::number(i_range).toLocal8Bit());
        reply->setRawHeader("Content-Type", "video/mp4");
        reply->setRawHeader("Content-Range", QString("bytes %1-%2/%3").arg(range).arg(filesize-1).arg(filesize).toLocal8Bit());

        _file->seek(range.toInt());
        reply->write(_file->read(i_range));
    }
    else
    {
        reply->setStatus(200);
        reply->setRawHeader("Content-Length", "2");
        reply->setRawHeader("Content-Type", "video/mp4");
        reply->write("\r\n");
    }
    reply->close();
}
