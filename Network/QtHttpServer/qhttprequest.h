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

#ifndef QHTTPREQUEST_H
#define QHTTPREQUEST_H

#include <QtCore/QBuffer>
#include <QtCore/QVariant>
#include <QtCore/QUuid>

class QHttpConnection;
class QNetworkCookie;

class QHttpFileData : public QBuffer
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged)
    Q_PROPERTY(QString contentType READ contentType NOTIFY contentTypeChanged)
public:
    QHttpFileData(const QHash<QByteArray, QByteArray> &rawHeaders, const QByteArray &data, QObject *parent = Q_NULLPTR);
    ~QHttpFileData();
    const QString &fileName() const;
    const QString &contentType() const;

Q_SIGNALS:
    void fileNameChanged(const QString &fileName);
    void contentTypeChanged(const QString &contentType);

private:
    class Private;
    Private *d;
    Q_DISABLE_COPY(QHttpFileData)
};

class QHttpRequest : public QBuffer
{
    Q_OBJECT
public:
    explicit QHttpRequest(QHttpConnection *parent);

    const QUuid &uuid() const;
    const QString &remoteAddress() const;
    const QByteArray &method() const;
    bool hasRawHeader(const QByteArray &headerName) const;
    QByteArray rawHeader(const QByteArray &headerName) const;
    QList<QByteArray> rawHeaderList() const;
    const QList<QNetworkCookie> &cookies() const;
    const QList<QHttpFileData *> &files() const;
    const QUrl &url() const;

public Q_SLOTS:
    void setUrl(const QUrl &url);

Q_SIGNALS:
    void urlChanged(const QUrl &url);
    void upgrade(const QByteArray &to, const QUrl &url, const QHash<QByteArray, QByteArray> &rawHeaders);
    void ready();

private:
    class Private;
    Private *d;
    Q_DISABLE_COPY(QHttpRequest)
};

QDebug operator<<(QDebug, const QHttpRequest *);


#endif // QHTTPREQUEST_H
