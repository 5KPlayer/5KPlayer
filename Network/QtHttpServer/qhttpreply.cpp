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

#include "qhttpreply.h"
#include "qhttpconnection.h"
#include "qhttprequest.h"

#include <QtNetwork/QNetworkCookie>

#include <zlib.h>

class QHttpReply::Private : public QObject
{
    Q_OBJECT
public:
    Private(QHttpConnection *c, QHttpReply *parent);

public slots:
    void writeHeaders();
    void writeBody();

private:
    QHttpReply *q;
    static QHash<int, QByteArray> statusCodes;

public:
    QHttpConnection *connection;
    int status;
    QHash<QByteArray, QByteArray> rawHeaders;
    QList<QNetworkCookie> cookies;
    QByteArray data;
};

QHash<int, QByteArray> QHttpReply::Private::statusCodes;

QHttpReply::Private::Private(QHttpConnection *c, QHttpReply *parent)
    : QObject(parent)
    , q(parent)
    , connection(c)
    , status(200)
{
    if (statusCodes.isEmpty()) {
        statusCodes.insert(100, "Continue");
        statusCodes.insert(101, "Switching Protocols");
        statusCodes.insert(200, "OK");
        statusCodes.insert(201, "Created");
        statusCodes.insert(202, "Accepted");
        statusCodes.insert(203, "Non-Authoritative Information");
        statusCodes.insert(204, "No Content");
        statusCodes.insert(205, "Reset Content");
        statusCodes.insert(206, "Partial Content");
        statusCodes.insert(300, "Multiple Choices");
        statusCodes.insert(301, "Moved Permanently");
        statusCodes.insert(302, "Found");
        statusCodes.insert(303, "See Other");
        statusCodes.insert(304, "Not Modified");
        statusCodes.insert(305, "Use Proxy");
        statusCodes.insert(307, "Temporary Redirect");
        statusCodes.insert(400, "Bad Request");
        statusCodes.insert(401, "Unauthorized");
        statusCodes.insert(402, "Payment Required");
        statusCodes.insert(403, "Forbidden");
        statusCodes.insert(404, "Not Found");
        statusCodes.insert(405, "Method Not Allowed");
        statusCodes.insert(406, "Not Acceptable");
        statusCodes.insert(407, "Proxy Authentication Required");
        statusCodes.insert(408, "Request Time-out");
        statusCodes.insert(409, "Conflict");
        statusCodes.insert(410, "Gone");
        statusCodes.insert(411, "Length Required");
        statusCodes.insert(412, "Precondition Failed");
        statusCodes.insert(413, "Request Entity Too Large");
        statusCodes.insert(414, "Request-URI Too Large");
        statusCodes.insert(415, "Unsupported Media Type");
        statusCodes.insert(416, "Requested range not satisfiable");
        statusCodes.insert(417, "Expectation Failed");
        statusCodes.insert(500, "Internal Server Error");
        statusCodes.insert(501, "Not Implemented");
        statusCodes.insert(502, "Bad Gateway");
        statusCodes.insert(503, "Service Unavailable");
        statusCodes.insert(504, "Gateway Time-out");
        statusCodes.insert(505, "HTTP Version not supported");
    }
    q->setBuffer(&data);
    q->open(QIODevice::WriteOnly);
}

void QHttpReply::Private::writeHeaders()
{
    connection->write("HTTP/1.1 ");
    connection->write(QByteArray::number(status));
    connection->write(" ");
    connection->write(statusCodes.value(status));
    connection->write("\r\n");
    const QHttpRequest *request = connection->requestFor(q);
    if (request && request->hasRawHeader("Accept-Encoding") && !rawHeaders.contains("Content-Encoding")) {
        QList<QByteArray> acceptEncodings;
        foreach (const QByteArray &acceptEncoding, request->rawHeader("Accept-Encoding").split(',')) {
            acceptEncodings.append(acceptEncoding.trimmed());
        }
        if (acceptEncodings.contains("deflate")) {
            z_stream z;
            z.zalloc = NULL;
            z.zfree = NULL;
            z.opaque = NULL;

            if (deflateInit(&z, Z_DEFAULT_COMPRESSION) == Z_OK) {
                QByteArray newData;
                unsigned char buf[1024];
                z.avail_in = data.size();
                z.next_in = reinterpret_cast<Bytef*>(data.data());
                z.avail_out = 1024;
                z.next_out = buf;
                int ret = Z_OK;
                while (ret == Z_OK) {
                    ret = deflate(&z, Z_FINISH);
                    if (ret == Z_STREAM_END) {
                        newData.append((const char*)buf, 1024 - z.avail_out);
                        data = newData;
                        rawHeaders["Content-Encoding"] = "deflate";
                        rawHeaders["Content-Length"] = QString::number(data.length()).toUtf8();
                        break;
                    } else if (ret != Z_OK) {
                        qWarning() << "deflate failed:" << ret << z.msg;
                    }
                    if (z.avail_out == 0) {
                        newData.append((const char*)buf, 1024);
                        z.avail_out = 1024;
                        z.next_out = buf;
                    }
                }
                deflateEnd(&z);
            }
        }
    }

    if (!rawHeaders.contains("Content-Length")) {
        rawHeaders.insert("Content-Length", QString::number(data.length()).toUtf8());
    }
    foreach (const QByteArray &rawHeader, rawHeaders.keys()) {
        connection->write(rawHeader);
        connection->write(": ");
        QByteArray value = rawHeaders.value(rawHeader);
        connection->write(value.replace('\r', "%0D").replace('\n', "%0A"));
        connection->write("\r\n");
    }

    foreach (const QNetworkCookie &cookie, cookies) {
        connection->write("Set-Cookie: ");
        QByteArray value = cookie.toRawForm();
        connection->write(value.replace('\r', "%0D").replace('\n', "%0A"));
        connection->write(";\r\n");
    }

    connection->write("\r\n");
}

void QHttpReply::Private::writeBody()
{
    connection->write(data);
    q->deleteLater();
}

QHttpReply::QHttpReply(QHttpConnection *parent)
    : QBuffer(parent)
    , d(new Private(parent, this))
{
}

QHttpReply::~QHttpReply()
{
    delete d;
}

int QHttpReply::status() const
{
    return d->status;
}

void QHttpReply::setStatus(int status)
{
    if (d->status == status) return;
    d->status = status;
    emit statusChanged(status);
}

bool QHttpReply::hasRawHeader(const QByteArray &headerName) const
{
    return d->rawHeaders.contains(headerName);
}

//QVariant QHttpReply::header(KnownHeaders header) const
//{
//    return QVariant();
//}

QByteArray QHttpReply::rawHeader(const QByteArray &headerName) const
{
    return d->rawHeaders.value(headerName);
}

void QHttpReply::setRawHeader(const QByteArray &headerName, const QByteArray &value)
{
    d->rawHeaders.insert(headerName, value);
}

QList<QByteArray> QHttpReply::rawHeaderList() const
{
    return d->rawHeaders.keys();
}

const QList<QNetworkCookie> &QHttpReply::cookies() const
{
    return d->cookies;
}

void QHttpReply::setCookies(const QList<QNetworkCookie> &cookies)
{
    if (d->cookies == cookies) return;
    d->cookies = cookies;
}

void QHttpReply::close()
{
    QBuffer::close();
//    QMetaObject::invokeMethod(d, "close", Qt::QueuedConnection);
    d->writeHeaders();
    d->writeBody();
}

#include "qhttpreply.moc"
