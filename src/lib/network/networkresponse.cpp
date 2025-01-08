#include "networkresponse.h"
#include "networkresponse_p.h"

#include <RestLink/compressionutils.h>

#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>

namespace RestLink {

NetworkResponse::NetworkResponse(Api *api)
    : Response(new NetworkResponsePrivate(this), api)
{
}

ApiBase::Operation NetworkResponse::operation() const
{
    RESTLINK_D(NetworkResponse);

    switch (d->netReply->operation()) {
    case QNetworkAccessManager::GetOperation:
        return Api::GetOperation;

    case QNetworkAccessManager::PostOperation:
        return Api::PostOperation;

    case QNetworkAccessManager::PutOperation:
        return Api::PutOperation;

    case QNetworkAccessManager::DeleteOperation:
        return Api::DeleteOperation;

    case QNetworkAccessManager::CustomOperation:
        return Api::PatchOperation;

    default:
        return Api::UnknownOperation;
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
    return d->netReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}

QString NetworkResponse::httpReasonPhrase() const
{
    RESTLINK_D(const NetworkResponse);
    return d->netReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
}

bool NetworkResponse::hasHeader(const QByteArray &name) const
{
    RESTLINK_D(const NetworkResponse);
    return d->netReply->hasRawHeader(name);
}

QByteArray NetworkResponse::header(const QByteArray &name) const
{
    RESTLINK_D(const NetworkResponse);
    return d->netReply->rawHeader(name);
}

QByteArrayList NetworkResponse::headerList() const
{
    RESTLINK_D(const NetworkResponse);
    return d->netReply->rawHeaderList();
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
    connect(reply, &QNetworkReply::readyRead, this, &Response::readyRead);
    connect(reply, &QNetworkReply::downloadProgress, this, &Response::downloadProgress);
    connect(reply, &QNetworkReply::uploadProgress, this, &Response::uploadProgress);
    connect(reply, &QNetworkReply::sslErrors, this, &Response::sslErrorsOccured);
    connect(reply, &QNetworkReply::errorOccurred, this, &Response::networkErrorOccured);
    connect(reply, &QNetworkReply::finished, this, &Response::finished);
}

NetworkResponsePrivate::NetworkResponsePrivate(Response *q) :
    ResponsePrivate(q),
    netReply(nullptr)
{
}

} // namespace RestLink
