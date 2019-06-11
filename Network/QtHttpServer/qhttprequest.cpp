/* Copyright (c) 2012 QtHttpServer Project.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the QtHttpServer nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL QTHTTPSERVER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "qhttprequest.h"
#include "qhttpconnection.h"

#include <QtCore/QUrl>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkCookie>


class QHttpFileData::Private
{
public:
    QString fileName;
    QString contentType;
    QByteArray data;
};

QHttpFileData::QHttpFileData(const QHash<QByteArray, QByteArray> &rawHeaders, const QByteArray &data, QObject *parent)
    : QBuffer(parent)
    , d(new Private)
{
    if (rawHeaders.contains("Content-Type")) {
        d->contentType = QString::fromUtf8(rawHeaders.value("Content-Type"));
    }
    if (rawHeaders.contains("Content-Disposition")) {
        QByteArray contentDisposition = rawHeaders.value("Content-Disposition");
        contentDisposition = contentDisposition.mid(contentDisposition.indexOf("filename=\"") + 10);
        contentDisposition.chop(1);
        d->fileName = QString::fromUtf8(contentDisposition);
    }
    d->data = data;
    setBuffer(&d->data);
    open(QIODevice::ReadOnly);
}

QHttpFileData::~QHttpFileData()
{
    delete d;
}

const QString &QHttpFileData::fileName() const
{
    return d->fileName;
}

const QString &QHttpFileData::contentType() const
{
    return d->contentType;
}


class QHttpRequest::Private : public QObject
{
    Q_OBJECT
public:
    enum ReadState {
        ReadUrl
        , ReadHeaders
        , ReadBody
        , MultipartHeader
        , MultipartBody
        , ReadDone
    };

    Private(QHttpConnection *c, QHttpRequest *parent);

private slots:
    void readyRead();
    void disconnected();

private:
    QHttpRequest *q;

public:
    QHttpConnection *connection;
    QUuid uuid;
    QString remoteAddress;
    ReadState state;
    QByteArray method;
    QUrl url;
    QHash<QByteArray, QByteArray> rawHeaders;
    QList<QNetworkCookie> cookies;
    QByteArray data;
    QByteArray multipartBoundary;
    QList<QHttpFileData *> files;
};

QHttpRequest::Private::Private(QHttpConnection *c, QHttpRequest *parent)
    : QObject(parent)
    , q(parent)
    , connection(c)
    , uuid(QUuid::createUuid())
    , remoteAddress(connection->peerAddress().toString())
    , state(ReadUrl)
{
    connect(connection, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(connection, SIGNAL(disconnected()), this, SLOT(disconnected()));
    q->setBuffer(&data);
    q->open(QIODevice::ReadOnly);
    QMetaObject::invokeMethod(this, "readyRead", Qt::QueuedConnection);
}

void QHttpRequest::Private::readyRead()
{
    switch (state) {
    case ReadUrl:
        if (connection->canReadLine()) {
            QByteArray line = connection->readLine();
            line = line.left(line.length() - 2);
            QList<QByteArray> array = line.split(' ');

            method = array.takeFirst();

            QString path = QString::fromUtf8(array.takeFirst());
            url.setPath(path.section('?', 0, 0), QUrl::StrictMode);
            url.setQuery(path.section('?', 1));
            url.setScheme(QLatin1String("http"));

            QByteArray http = array.takeFirst();
            if (http != "HTTP/1.1" && http != "HTTP/1.0") {
                qWarning() << http << "is not supported.";
                connection->disconnectFromHost();
                return;
            }
            state = ReadHeaders;
        }
//        break;
    case ReadHeaders:
        while (connection->canReadLine()) {
            QByteArray line = connection->readLine();
            line = line.left(line.length() - 2);
            if (line.isEmpty()) {
                if (!q->hasRawHeader("Content-Length")) {
                    state = ReadDone;
                    disconnect(connection, SIGNAL(readyRead()), this, SLOT(readyRead()));
                    emit q->ready();
                } else {
                    state = ReadBody;
                    QMetaObject::invokeMethod(this, "readyRead", Qt::QueuedConnection);
                }
                break;
            }

            int space = line.indexOf(' ');
            if (space > 0) {
                QByteArray name = line.left(space - 1);
                QByteArray value = line.mid(space + 1);
                if (name == "Upgrade") {
                    disconnect(connection, 0, this, 0);
                    emit q->upgrade(value, url, rawHeaders);
                    return;
                } else if (name == "Host") {
                    int colon = value.indexOf(':');
                    if (colon > -1) {
                        url.setHost(QString::fromUtf8(value.left(colon)));
                        url.setPort(value.mid(colon + 1).toUInt());
                    } else {
                        url.setHost(QString::fromUtf8(value));
                        url.setPort(80);
                    }
                } else if (name == "Cookie") {
                    foreach (const QByteArray &c, value.split(';')) {
                        cookies.append(QNetworkCookie::parseCookies(c));
                    }
                } else if (name == "Content-Type") {
                    QList<QByteArray> fields = value.split(';');
                    QByteArray boundary(" boundary=");
                    if (fields.first().toLower() == "multipart/form-data" && fields.length() == 2 && fields.at(1).startsWith(boundary)) {
                        rawHeaders.insert(name.toLower(), fields.takeFirst().toLower());
                        multipartBoundary = fields.takeFirst().mid(boundary.length());
                        multipartBoundary.prepend("--");
                    } else {
                        rawHeaders.insert(name.toLower(), value);
                    }
                } else {
                    rawHeaders.insert(name.toLower(), value);
                }
            }
        }
        break;
    case ReadBody:
        if (q->hasRawHeader("Content-Length")) {
            int length = q->rawHeader("Content-Length").toLongLong();
            data.append(connection->read(length - data.length()));
            if (data.length() == length) {
                QHash<QByteArray, QByteArray> multipartRawHeaders;
                QByteArray multipartData;
                if (!multipartBoundary.isEmpty()) {
                    QByteArray newData;
                    foreach (QByteArray ba, data.split('\n')) {
                        switch (state) {
                        case ReadBody:
                            ba.chop(1); // \r
                            if (ba == multipartBoundary) {
                                state = MultipartHeader;
                            } else {
                                qWarning() << ba << multipartBoundary;
                            }
                            break;
                        case MultipartHeader:
                            ba.chop(1); // \r
                            if (ba.isEmpty()) {
                                state = MultipartBody;
                            } else {
                                int i = ba.indexOf(':');
                                multipartRawHeaders.insert(ba.left(i), ba.mid(i + 2));
                            }
                            break;
                        case MultipartBody:
                            if (ba.startsWith(multipartBoundary)) {
                                ba.chop(1); // \r
                                if (multipartRawHeaders.contains("Content-Type")) {
                                    if (multipartData.size() > 0
                                            && multipartRawHeaders.contains("Content-Disposition")
                                            && !rawHeaders.value("Content-Disposition").contains("filename=\"\"")) {
                                        files.append(new QHttpFileData(multipartRawHeaders, multipartData, this));
                                    }
                                } else {
                                    QByteArray name = multipartRawHeaders.value("Content-Disposition").split('=').at(1);
                                    name = name.mid(1, name.length() - 2);
                                    if (!newData.isEmpty()) {
                                        newData.append("&");
                                    }
                                    newData.append(name);
                                    newData.append("=");
                                    multipartData.chop(2);
                                    newData.append(QUrl::toPercentEncoding(QString::fromUtf8(multipartData)));
                                }
                                multipartRawHeaders.clear();
                                multipartData.clear();
                                if (ba.endsWith("--")) {
                                    data = newData;
                                    state = ReadDone;
                                } else {
                                    state = MultipartHeader;
                                }
                            } else {
                                multipartData.append(ba);
                                multipartData.append("\n");
                            }
                            break;
                        default:
                            break;
                        }
                    }
                }
                state = ReadDone;
                emit q->ready();
            }
        }
        break;
    default:
        break;
    }
}

void QHttpRequest::Private::disconnected()
{
    q->deleteLater();
}

QHttpRequest::QHttpRequest(QHttpConnection *parent)
    : QBuffer(parent)
    , d(new Private(parent, this))
{
}

const QUuid &QHttpRequest::uuid() const
{
    return d->uuid;
}

const QString &QHttpRequest::remoteAddress() const
{
    return d->remoteAddress;
}

const QByteArray &QHttpRequest::method() const
{
    return d->method;
}

bool QHttpRequest::hasRawHeader(const QByteArray &headerName) const
{
    return d->rawHeaders.contains(headerName.toLower());
}

QByteArray QHttpRequest::rawHeader(const QByteArray &headerName) const
{
    return d->rawHeaders.value(headerName.toLower());
}

QList<QByteArray> QHttpRequest::rawHeaderList() const
{
    return d->rawHeaders.keys();
}

const QList<QNetworkCookie> &QHttpRequest::cookies() const
{
    return d->cookies;
}

const QList<QHttpFileData *> &QHttpRequest::files() const
{
    return d->files;
}

const QUrl &QHttpRequest::url() const
{
    return d->url;
}

void QHttpRequest::setUrl(const QUrl &url)
{
    if (d->url == url) return;
    d->url = url;
    emit urlChanged(url);
}

QDebug operator<<(QDebug dbg, const QHttpRequest *request)
{
    if (!request) {
        return dbg << "QHttpRequest {}";
    }
    QDebugStateSaver saver(dbg);
    dbg.resetFormat();
    dbg.nospace();
    dbg << "QHttpRequest ";
    dbg << "{ uuid: " << request->uuid().toString();
    dbg << "; method: " << request->method();
    dbg << "; url: " << request->url().toString();
    if (request->hasRawHeader("X-Forwarded-For")) {
        dbg << "; ip: " << request->rawHeader("X-Forwarded-For");
    } else {
        dbg << "; ip: " << request->remoteAddress();
    }
    if (request->hasRawHeader("User-Agent")) {
        dbg << "; user_agent: " << request->rawHeader("User-Agent");
    }
    if (request->hasRawHeader("Referer")) {
        dbg << "; referer: " << request->rawHeader("Referer");
    }
    dbg << " }";
    return dbg;
}

#include "qhttprequest.moc"
