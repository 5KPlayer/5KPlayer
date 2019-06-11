#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "singleton.h"
#include <QObject>
#include <QtNetwork>

#define Http  HttpRequest::getInstance()

class HttpRequest : public QObject, public Singleton<HttpRequest>
{
    Q_OBJECT
    friend class Singleton<HttpRequest>;
    friend class QSharedPointer<HttpRequest>;

public:
    void setProxy(const QNetworkProxy &proxy);

    QNetworkReply *get(const QNetworkRequest &request);
    QNetworkReply *get(const QString &url);

    QNetworkReply *post(const QNetworkRequest &request, const QByteArray &data);
    QNetworkReply *post(const QString &url, const QByteArray &data);

    void stopRequest(QNetworkReply *reply);

private:
    Q_DISABLE_COPY(HttpRequest)
    explicit HttpRequest(QObject *parent = 0);

    QNetworkAccessManager *p_manager;
};

#endif // HTTPREQUEST_H
