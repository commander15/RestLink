#include "apireply.h"
#include "apireply_p.h"

#include <RestLink/debug.h>
#include <RestLink/api.h>
#include <RestLink/compressionutils.h>

#include <QtNetwork/qnetworkreply.h>

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace RestLink {

ApiReply::ApiReply(Api *api) :
    QObject(api),
    d(new ApiReplyPrivate(this))
{
    d->api = api;
}

ApiReply::ApiReply(ApiReplyPrivate *dd, Api *api) :
    QObject(api),
    d(dd)
{
    d->api = api;
}

ApiReply::~ApiReply()
{
}

QString ApiReply::endpoint() const
{
    return d->apiRequest.endpoint();
}

ApiRequest ApiReply::apiRequest() const
{
    return d->apiRequest;
}

ApiBase::Operation ApiReply::operation() const
{
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
    default:
        return Api::PatchOperation;
    }
}

Api *ApiReply::api() const
{
    return d->api;
}

QUrl ApiReply::url() const
{
    return d->netReply->request().url();
}

QNetworkRequest ApiReply::networkRequest() const
{
    return d->netReply->request();
}

bool ApiReply::isHttpStatusSuccess() const
{
    const int code = httpStatusCode();
    return (code >= 200 && code <= 299);
}

int ApiReply::httpStatusCode() const
{
    return d->netReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}

QString ApiReply::httpReasonPhrase() const
{
    return d->netReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
}

bool ApiReply::hasHeader(const QByteArray &name) const
{
    return d->netReply->hasRawHeader(name);
}

QByteArray ApiReply::header(const QByteArray &header) const
{
    return d->netReply->rawHeader(header);
}

QByteArrayList ApiReply::headerList() const
{
    return d->netReply->rawHeaderList();
}

bool ApiReply::isRunning() const
{
    return d->netReply->isRunning();
}

bool ApiReply::isFinished() const
{
    return d->netReply->isFinished();
}

void ApiReply::abort()
{
    d->netReply->abort();
}

bool ApiReply::isSuccess() const
{
    if (isFinished())
        return !hasNetworkError() && isHttpStatusSuccess();
    else
        return false;
}

QJsonObject ApiReply::readJsonObject(QJsonParseError *error)
{
    return readJson(error).toObject();
}

QJsonArray ApiReply::readJsonArray(QJsonParseError *error)
{
    return readJson(error).toArray();
}

QJsonValue ApiReply::readJson(QJsonParseError *error)
{
    const QJsonDocument doc = QJsonDocument::fromJson(readBody(), error);
    if (doc.isObject())
        return doc.object();
    else if (doc.isArray())
        return doc.array();
    else
        return QJsonValue();
}

QString ApiReply::readString()
{
    return QString::fromUtf8(readBody());
}

QByteArray ApiReply::readBody()
{
    if (d->netReply->bytesAvailable() > 0) {
        const QByteArray data = d->netReply->readAll();
        if (d->netReply->hasRawHeader("Content-Encoding"))
            return CompressionUtils::decompress(data, d->netReply->rawHeader("Content-Encoding"));
        else
            return data;
    } else
        return QByteArray();
}

void ApiReply::ignoreSslErros(const QList<QSslError> &errors)
{
    d->netReply->ignoreSslErrors(errors);
}

bool ApiReply::hasNetworkError() const
{
    return d->netReply->error() != QNetworkReply::NoError;
}

int ApiReply::networkError() const
{
    return d->netReply->error();
}

QString ApiReply::networkErrorString() const
{
    return d->netReply->errorString();
}

QNetworkReply *ApiReply::networkReply() const
{
    return d->netReply;
}

void ApiReply::setApiRequest(const ApiRequest &request)
{
    d->apiRequest = request;
}

void ApiReply::setNetworkReply(QNetworkReply *reply)
{
    d->netReply = reply;
    d->netReply->setParent(this);

    connect(reply, &QNetworkReply::readyRead, this, &ApiReply::readyRead);
    connect(reply, &QNetworkReply::downloadProgress, this, &ApiReply::downloadProgress);
    connect(reply, &QNetworkReply::uploadProgress, this, &ApiReply::uploadProgress);
    connect(reply, &QNetworkReply::sslErrors, this, &ApiReply::sslErrorsOccured);
    connect(reply, &QNetworkReply::errorOccurred, this, &ApiReply::networkErrorOccured);
    connect(reply, &QNetworkReply::finished, this, &ApiReply::finished);
}

ApiReplyPrivate::ApiReplyPrivate(ApiReply *qq) :
    q(qq),
    api(nullptr),
    netReply(nullptr)
{
}

}
