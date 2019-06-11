#include "httprequest.h"

HttpRequest::HttpRequest(QObject *parent) : QObject(parent)
{
    p_manager = new QNetworkAccessManager(this);
}

void HttpRequest::setProxy(const QNetworkProxy &proxy)
{
    p_manager->setProxy(proxy);
}

QNetworkReply *HttpRequest::get(const QNetworkRequest &request)
{
    return p_manager->get(request);
}

QNetworkReply *HttpRequest::get(const QString &url)
{
    return p_manager->get(QNetworkRequest(QUrl(url)));
}

QNetworkReply *HttpRequest::post(const QNetworkRequest &request, const QByteArray &data)
{
    return p_manager->post(request, data);
}

QNetworkReply *HttpRequest::post(const QString &url, const QByteArray &data)
{
    return p_manager->post(QNetworkRequest(QUrl(url)), data);
}

void HttpRequest::stopRequest(QNetworkReply *reply)
{
    if(reply) {
        reply->disconnect();
        reply->abort();
        reply->deleteLater();
    }
    reply = NULL;
}
