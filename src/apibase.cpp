#include "apibase.h"
#include "apibase_p.h"

#include <RestLink/debug.h>
#include <RestLink/apirequest.h>
#include <RestLink/private/apirequest_p.h>
#include <RestLink/apireply.h>

#include <QtNetwork/qsslsocket.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkrequest.h>

#include <QtCore/qurl.h>
#include <QtCore/qurlquery.h>
#include <QtCore/qfile.h>

namespace RestLink {

ApiBase::ApiBase(QObject *parent) :
    QObject(parent),
    d(new ApiBasePrivate(this))
{
    d->netMan = new QNetworkAccessManager(this);
    d->netMan->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
}

ApiBase::ApiBase(QNetworkAccessManager *networkManager, QObject *parent) :
    QObject(parent),
    d(new ApiBasePrivate(this))
{
    d->netMan = networkManager;
}

ApiBase::ApiBase(ApiBasePrivate *dd, QObject *parent) :
    QObject(parent),
    d(dd)
{
    if (!d->netMan)
        d->netMan = new QNetworkAccessManager(this);
}

ApiBase::~ApiBase()
{
}

QList<ApiRequestParameter> ApiBase::apiParameters() const
{
    return QList<ApiRequestParameter>();
}

ApiReply *ApiBase::exec(const ApiRequest &request)
{
    d->logRequest(request);

    QByteArray verb;
    switch (request.verb()) {
    case ApiRequest::GetRequest:
        verb = "GET";
        break;

    case ApiRequest::PostRequest:
        verb = "POST";
        break;

    case ApiRequest::PutRequest:
        verb = "PUT";
        break;

    case ApiRequest::PatchRequest:
        verb = "PATCH";
        break;

    case ApiRequest::DeleteRequest:
        verb = "DELETE";
        break;
    }

    QNetworkRequest netReq = createNetworkRequest(request);
    QNetworkReply *netReply = createNetworkReply(verb, netReq, request.data());

    ApiReply *reply = new ApiReply(this);
    reply->setApiRequest(request);
    reply->setNetworkReply(netReply);
    return reply;
}

QNetworkAccessManager *ApiBase::networkAccessManager() const
{
    return d->netMan;
}

QNetworkRequest ApiBase::createNetworkRequest(const ApiRequest &request)
{
    QNetworkRequest netReq;
    netReq.setHeader(QNetworkRequest::UserAgentHeader, userAgent());
    netReq.setRawHeader("Accept", "application/json");
    netReq.setRawHeader("Connection", "keep-alive");

    QUrl url(apiUrl());
    url.setPath(url.path() + request.endpoint());

    QUrlQuery urlQuery(url.query());

    const QList<ApiRequestParameter> parameters = apiParameters() + request.d->parameters;
    for (const ApiRequestParameter &parameter : parameters) {
        switch (parameter.scope()) {
        case ApiRequestParameter::UrlPathScope:
            url.setPath(url.path() + '/' + parameter.value().toString());
            break;

        case ApiRequestParameter::UrlQueryScope:
            urlQuery.addQueryItem(parameter.name(), parameter.value().toString());
            break;

        case ApiRequestParameter::HeaderScope:
            netReq.setRawHeader(parameter.name().toUtf8(), parameter.value().toByteArray());
            break;
        }
    }

    setupUrlQuery(&urlQuery, request);

    url.setQuery(urlQuery);
    setupUrl(&url, request);

    netReq.setUrl(url);
    setupNetworkRequest(&netReq, request);

    return netReq;
}

QNetworkReply *ApiBase::createNetworkReply(const QByteArray &verb, const QNetworkRequest &request, const QByteArray &data, int dataType)
{
    if (dataType == 0)
        return d->netMan->sendCustomRequest(request, verb, data);
    else {
        QFile *file = new QFile(data, this);
        file->open(QIODevice::ReadOnly);

        QNetworkReply *reply = d->netMan->sendCustomRequest(request, verb, file);
        file->setParent(reply);
        return reply;
    }
}

void ApiBase::setupNetworkRequest(QNetworkRequest *netReq, const ApiRequest &apiReq)
{
}

void ApiBase::setupUrl(QUrl *url, const ApiRequest &apiReq)
{
}

void ApiBase::setupUrlQuery(QUrlQuery *url, const ApiRequest &apiReq)
{
}

ApiBasePrivate::ApiBasePrivate(ApiBase *qq) :
    q(qq),
    netMan(nullptr)
{
    if (!QSslSocket::supportsSsl())
        restlinkWarning() << "SSL support unavailable";
}

ApiBasePrivate::~ApiBasePrivate()
{
}

void ApiBasePrivate::logRequest(const ApiRequest &request)
{
    QString type;
    switch (request.verb()) {
    case ApiRequest::GetRequest:
        type = QStringLiteral("GET");
        break;

    case ApiRequest::PostRequest:
        type = QStringLiteral("POST");
        break;

    case ApiRequest::PutRequest:
        type = QStringLiteral("PUT");
        break;

    case ApiRequest::DeleteRequest:
        type = QStringLiteral("DELETE");
        break;
    }

    QNetworkRequest netReq = q->createNetworkRequest(request);

    restlinkInfo() << type << ' ' << netReq.url().toString();
}

void ApiBasePrivate::logRequest(const QNetworkRequest &request)
{
    restlinkInfo() << "GET " << request.url().toString();
}

}
