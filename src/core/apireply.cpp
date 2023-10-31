#include "apireply.h"
#include "apireply_p.h"

#include <RestLink/debug.h>
#include <RestLink/api.h>

#include <QtNetwork/qnetworkreply.h>

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

ApiRequest ApiReply::apiRequest() const
{
    return d->apiRequest;
}

Api *ApiReply::api() const
{
    return d->api;
}

QByteArray ApiReply::downloadedData() const
{
    if (d->netReply->bytesAvailable() > 0)
        d->netData.append(d->netReply->readAll());
    return d->netData;
}

int ApiReply::networkError() const
{
    return d->netReply->error();
}

QString ApiReply::networkErrorString() const
{
    return d->netReply->errorString();
}

void ApiReply::abort()
{
    d->netReply->abort();
}

int ApiReply::httpStatusCode() const
{
    return d->netReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}

QString ApiReply::httpReasonPhrase() const
{
    return d->netReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
}

QUrl ApiReply::requestedUrl() const
{
    return d->netReply->request().url();
}

QNetworkRequest ApiReply::networkRequest() const
{
    return d->netReply->request();
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

    connect(reply, &QNetworkReply::downloadProgress, this, &ApiReply::downloadProgress);
    connect(reply, &QNetworkReply::uploadProgress, this, &ApiReply::uploadProgress);
    connect(reply, &QNetworkReply::finished, this, &ApiReply::completeDownload);
    connect(reply, &QNetworkReply::errorOccurred, this, &ApiReply::networkErrorOccured);
    connect(reply, &QIODevice::readyRead, this, &ApiReply::downloadPart);
}

void ApiReply::processData()
{
    if (d->netData.isEmpty())
        restlinkWarning() << "received an empty response";
}

void ApiReply::downloadPart()
{
    d->netData.append(d->netReply->readAll());
}

void ApiReply::completeDownload()
{
    if (networkError() != QNetworkReply::NoError)
        restlinkWarning() << networkErrorString();
    else if (httpStatusCode() != 200)
        restlinkWarning() << "HTTP " << httpStatusCode() << ' '
                          << httpReasonPhrase();

    if (!d->netData.isEmpty())
        processData();

    emit finished();
}

ApiReplyPrivate::ApiReplyPrivate(ApiReply *qq) :
    q(qq),
    api(nullptr),
    netReply(nullptr)
{
}

}
