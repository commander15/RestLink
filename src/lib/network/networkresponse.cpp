#include "networkresponse.h"
#include "networkresponse_p.h"

#include <RestLink/compressionutils.h>

#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>

namespace RestLink {

NetworkResponse::NetworkResponse(QObject *parent)
    : Response(new NetworkResponsePrivate(this), parent)
{
}

AbstractRequestHandler::Method NetworkResponse::method() const
{
    RESTLINK_D(NetworkResponse);

    switch (d->netReply->operation()) {
    case QNetworkAccessManager::GetOperation:
        return AbstractRequestHandler::GetMethod;

    case QNetworkAccessManager::PostOperation:
        return AbstractRequestHandler::PostMethod;

    case QNetworkAccessManager::PutOperation:
        return AbstractRequestHandler::PutMethod;

    case QNetworkAccessManager::DeleteOperation:
        return AbstractRequestHandler::DeleteMethod;

    case QNetworkAccessManager::CustomOperation:
        return AbstractRequestHandler::PatchMethod;

    default:
        return AbstractRequestHandler::UnknownMethod;
    }
}

QNetworkRequest NetworkResponse::networkRequest() const
{
    RESTLINK_D(const NetworkResponse);
    return d->netReply->request();
}

bool NetworkResponse::isFinished() const
{
    RESTLINK_D(const NetworkResponse);
    return d->netReply->isFinished();
}

int NetworkResponse::httpStatusCode() const
{
    RESTLINK_D(const NetworkResponse);

    // Return HTTP 200 instead of 0 for file scheme when file is accessible and readable
    if (d->netReply->url().scheme() == "file") {
        return (d->netReply->error() == QNetworkReply::NoError ? 200 : 0);
    }

    return d->netReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}

QString NetworkResponse::httpReasonPhrase() const
{
    RESTLINK_D(const NetworkResponse);
    return d->netReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
}

bool NetworkResponse::hasHeader(const QString &name) const
{
    RESTLINK_D(const NetworkResponse);
    return d->netReply->hasRawHeader(name);
}

QString NetworkResponse::header(const QString &name) const
{
    RESTLINK_D(const NetworkResponse);
    return d->netReply->rawHeader(name);
}

QStringList NetworkResponse::headerList() const
{
    RESTLINK_D(const NetworkResponse);

    const QByteArrayList rawHeaderNames = d->netReply->rawHeaderList();
    QStringList headerNames;
    std::transform(rawHeaderNames.begin(), rawHeaderNames.end(), std::back_inserter(headerNames), [](const QByteArray &rawHeaderName) {
        return QString::fromUtf8(rawHeaderName);
    });
    return headerNames;
}

QByteArray NetworkResponse::readBody()
{
    RESTLINK_D(NetworkResponse);
    if (d->netReply->bytesAvailable() > 0) {
        const QByteArray data = d->netReply->readAll();
        if (d->netReply->hasRawHeader("Content-Encoding"))
            return CompressionUtils::decompress(data, d->netReply->rawHeader("Content-Encoding"));
        else
            return data;
    } else
        return QByteArray();
}

int NetworkResponse::networkError() const
{
    RESTLINK_D(const NetworkResponse);
    return d->netReply->error();
}

QString NetworkResponse::networkErrorString() const
{
    RESTLINK_D(const NetworkResponse);
    return d->netReply->errorString();
}

QNetworkReply *NetworkResponse::networkReply() const
{
    RESTLINK_D(const NetworkResponse);
    return d->netReply;
}

void NetworkResponse::ignoreSslErrors()
{
    RESTLINK_D(NetworkResponse);
    d->netReply->ignoreSslErrors();
}

void NetworkResponse::abort()
{
    RESTLINK_D(NetworkResponse);
    d->netReply->abort();
}

void NetworkResponse::setReply(QNetworkReply *reply)
{
    RESTLINK_D(NetworkResponse);
    d->netReply = reply;

    reply->setParent(this);
    connect(reply, &QNetworkReply::downloadProgress, this, &Response::downloadProgress);
    connect(reply, &QNetworkReply::uploadProgress, this, &Response::uploadProgress);
#ifndef QT_NO_SSL
    connect(reply, &QNetworkReply::sslErrors, this, &Response::sslErrorsOccured);
#endif
    connect(reply, &QNetworkReply::errorOccurred, this, &Response::networkErrorOccured);
    connect(reply, &QNetworkReply::finished, this, &Response::finished);

    setResponseDevice(reply);
}

NetworkResponsePrivate::NetworkResponsePrivate(Response *q) :
    ResponsePrivate(q),
    netReply(nullptr)
{
}

} // namespace RestLink
