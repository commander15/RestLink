#include "apibase.h"
#include "apibase_p.h"

#include <RestLink/debug.h>
#include <RestLink/apirequest.h>
#include <RestLink/apireply.h>
#include <RestLink/jsonutils.h>

#include <RestLink/private/apirequest_p.h>

#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>

#include <QtCore/qmimedatabase.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qurlquery.h>

namespace RestLink {

ApiBase::ApiBase(ApiBasePrivate *d, QObject *parent) :
    QObject(parent),
    d_ptr(d)
{
}

ApiBase::~ApiBase()
{
}

void ApiBase::get(const ApiRequest &request, std::function<ApiRunCallback> callback)
{
    ApiReply *reply = get(request);
    connect(reply, &ApiReply::finished, this, [callback, reply] {
        callback(reply);
        reply->deleteLater();
    });
}

void ApiBase::post(const ApiRequest &request, QIODevice *device, std::function<ApiRunCallback> callback)
{
    ApiReply *reply = post(request, device);
    connect(reply, &ApiReply::finished, this, [callback, reply] {
        callback(reply);
        reply->deleteLater();
    });
}

void ApiBase::post(const ApiRequest &request, const QByteArray &data, std::function<ApiRunCallback> callback)
{
    ApiReply *reply = post(request, data);
    connect(reply, &ApiReply::finished, this, [callback, reply] {
        callback(reply);
        reply->deleteLater();
    });
}

void ApiBase::post(const ApiRequest &request, QHttpMultiPart *data, std::function<ApiRunCallback> callback)
{
    ApiReply *reply = post(request, data);
    connect(reply, &ApiReply::finished, this, [callback, reply] {
        callback(reply);
        reply->deleteLater();
    });
}

void ApiBase::post(const ApiRequest &request, const QJsonValue &data, std::function<ApiRunCallback> callback)
{
    ApiReply *reply = post(request, data);
    connect(reply, &ApiReply::finished, this, [callback, reply] {
        callback(reply);
        reply->deleteLater();
    });
}

void ApiBase::put(const ApiRequest &request, QIODevice *device, std::function<ApiRunCallback> callback)
{
    ApiReply *reply = put(request, device);
    connect(reply, &ApiReply::finished, this, [callback, reply] {
        callback(reply);
        reply->deleteLater();
    });
}

void ApiBase::put(const ApiRequest &request, const QByteArray &data, std::function<ApiRunCallback> callback)
{
    ApiReply *reply = put(request, data);
    connect(reply, &ApiReply::finished, this, [callback, reply] {
        callback(reply);
        reply->deleteLater();
    });
}

void ApiBase::put(const ApiRequest &request, QHttpMultiPart *data, std::function<ApiRunCallback> callback)
{
    ApiReply *reply = put(request, data);
    connect(reply, &ApiReply::finished, this, [callback, reply] {
        callback(reply);
        reply->deleteLater();
    });
}

void ApiBase::put(const ApiRequest &request, const QJsonValue &data, std::function<ApiRunCallback> callback)
{
    ApiReply *reply = put(request, data);
    connect(reply, &ApiReply::finished, this, [callback, reply] {
        callback(reply);
        reply->deleteLater();
    });
}

void ApiBase::patch(const ApiRequest &request, QIODevice *device, std::function<ApiRunCallback> callback)
{
    ApiReply *reply = patch(request, device);
    connect(reply, &ApiReply::finished, this, [callback, reply] {
        callback(reply);
        reply->deleteLater();
    });
}

void ApiBase::patch(const ApiRequest &request, const QByteArray &data, std::function<ApiRunCallback> callback)
{
    ApiReply *reply = patch(request, data);
    connect(reply, &ApiReply::finished, this, [callback, reply] {
        callback(reply);
        reply->deleteLater();
    });
}

void ApiBase::patch(const ApiRequest &request, QHttpMultiPart *data, std::function<ApiRunCallback> callback)
{
    ApiReply *reply = patch(request, data);
    connect(reply, &ApiReply::finished, this, [callback, reply] {
        callback(reply);
        reply->deleteLater();
    });
}

void ApiBase::patch(const ApiRequest &request, const QJsonValue &data, std::function<ApiRunCallback> callback)
{
    ApiReply *reply = patch(request, data);
    connect(reply, &ApiReply::finished, this, [callback, reply] {
        callback(reply);
        reply->deleteLater();
    });
}

void ApiBase::deleteResource(const ApiRequest &request, std::function<ApiRunCallback> callback)
{
    ApiReply *reply = deleteResource(request);
    connect(reply, &ApiReply::finished, this, [callback, reply] {
        callback(reply);
        reply->deleteLater();
    });
}

ApiReply *ApiBase::get(const ApiRequest &request)
{
    const QNetworkRequest netRequest = createNetworkRequest(request);
    QNetworkReply *netReply = d_ptr->netMan()->get(netRequest);
    return createApiReply(request, netReply);
}

ApiReply *ApiBase::post(const ApiRequest &request, QIODevice *device)
{
    const QNetworkRequest netRequest = createNetworkRequest(request, device, DeviceData);
    QNetworkReply *netReply = d_ptr->netMan()->post(netRequest, device);
    return createApiReply(request, netReply);
}

ApiReply *ApiBase::post(const ApiRequest &request, const QByteArray &data)
{
    const QNetworkRequest netRequest = createNetworkRequest(request, &data, RawData);
    QNetworkReply *netReply = d_ptr->netMan()->post(netRequest, data);
    return createApiReply(request, netReply);
}

ApiReply *ApiBase::post(const ApiRequest &request, QHttpMultiPart *data)
{
    const QNetworkRequest netRequest = createNetworkRequest(request, data, MultiPartData);
    QNetworkReply *netReply = d_ptr->netMan()->post(netRequest, data);
    return createApiReply(request, netReply);
}

ApiReply *ApiBase::post(const ApiRequest &request, const QJsonValue &data)
{
    const QNetworkRequest netRequest = createNetworkRequest(request, &data, JsonData);
    QNetworkReply *netReply = d_ptr->netMan()->post(netRequest, JsonUtils::jsonToByteArray(data));
    return createApiReply(request, netReply);
}

ApiReply *ApiBase::put(const ApiRequest &request, QIODevice *device)
{
    const QNetworkRequest netRequest = createNetworkRequest(request, device, DeviceData);
    QNetworkReply *netReply = d_ptr->netMan()->put(netRequest, device);
    return createApiReply(request, netReply);
}

ApiReply *ApiBase::put(const ApiRequest &request, const QByteArray &data)
{
    const QNetworkRequest netRequest = createNetworkRequest(request, &data, RawData);
    QNetworkReply *netReply = d_ptr->netMan()->put(netRequest, data);
    return createApiReply(request, netReply);
}

ApiReply *ApiBase::put(const ApiRequest &request, QHttpMultiPart *data)
{
    const QNetworkRequest netRequest = createNetworkRequest(request, data, MultiPartData);
    QNetworkReply *netReply = d_ptr->netMan()->put(netRequest, data);
    return createApiReply(request, netReply);
}

ApiReply *ApiBase::put(const ApiRequest &request, const QJsonValue &data)
{
    const QNetworkRequest netRequest = createNetworkRequest(request, &data, JsonData);
    QNetworkReply *netReply = d_ptr->netMan()->put(netRequest, JsonUtils::jsonToByteArray(data));
    return createApiReply(request, netReply);
}

ApiReply *ApiBase::patch(const ApiRequest &request, QIODevice *device)
{
    const QNetworkRequest netRequest = createNetworkRequest(request, device, DeviceData);
    QNetworkReply *netReply = d_ptr->netMan()->sendCustomRequest(netRequest, "PATCH", device);
    return createApiReply(request, netReply);
}

ApiReply *ApiBase::patch(const ApiRequest &request, const QByteArray &data)
{
    const QNetworkRequest netRequest = createNetworkRequest(request, &data, RawData);
    QNetworkReply *netReply = d_ptr->netMan()->sendCustomRequest(netRequest, "PATCH", data);
    return createApiReply(request, netReply);
}

ApiReply *ApiBase::patch(const ApiRequest &request, QHttpMultiPart *data)
{
    const QNetworkRequest netRequest = createNetworkRequest(request, data, MultiPartData);
    QNetworkReply *netReply = d_ptr->netMan()->sendCustomRequest(netRequest, "PATCH", data);
    return createApiReply(request, netReply);
}

ApiReply *ApiBase::patch(const ApiRequest &request, const QJsonValue &data)
{
    const QNetworkRequest netRequest = createNetworkRequest(request, &data, JsonData);
    QNetworkReply *netReply = d_ptr->netMan()->sendCustomRequest(netRequest, "PATCH", JsonUtils::jsonToByteArray(data));
    return createApiReply(request, netReply);
}

ApiReply *ApiBase::deleteResource(const ApiRequest &request)
{
    const QNetworkRequest netRequest = createNetworkRequest(request);
    QNetworkReply *netReply = d_ptr->netMan()->deleteResource(netRequest);
    return createApiReply(request, netReply);
}

QNetworkAccessManager *ApiBase::networkAccessManager() const
{
    return d_ptr->netMan();
}

void ApiBase::setNetworkAccessManager(QNetworkAccessManager *manager)
{
    d_ptr->setNetMan(manager);
}

QNetworkRequest ApiBase::createNetworkRequest(const ApiRequest &request, const void *data, DataType dataType)
{
    QNetworkRequest netReq;
    netReq.setOriginatingObject(this);

    netReq.setHeader(QNetworkRequest::UserAgentHeader, userAgent());
    if (!request.contentType().isEmpty()) {
        netReq.setHeader(QNetworkRequest::ContentTypeHeader, request.contentType());
    } else {
        static QMimeDatabase db;
        QMimeType type;

        switch (dataType) {
        case DeviceData:
            type = db.mimeTypeForData(static_cast<QIODevice *>(const_cast<void *>(data)));
            break;

        case RawData:
            type = db.mimeTypeForData(*static_cast<const QByteArray *>(data));
            break;

        case MultiPartData:
            break;

        case JsonData:
            type = db.mimeTypeForName("application/json; charset=UTF-8");
            break;

        default:
            break;
        }

        if (type.isValid())
            netReq.setHeader(QNetworkRequest::ContentTypeHeader, type.name().toLatin1());
    }

    netReq.setRawHeader("Accept", "*/*");
    netReq.setRawHeader("Accept-Encoding", "gzip, deflate");
    //netReq.setRawHeader("Accept-Language", QLocale().bcp47Name().toLatin1());
    netReq.setRawHeader("Connection", "keep-alive");

    if (request.isCacheable()) {
        netReq.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        netReq.setAttribute(QNetworkRequest::CacheSaveControlAttribute, true);
    } else {
        netReq.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
        netReq.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);
    }

    QUrl url = apiUrl();
    QString urlPath = url.path() + request.endpoint();
    QUrlQuery urlQuery(url.query());

    const QList<ApiRequestParameter> parameters = apiParameters() + request.parameters();
    for (const ApiRequestParameter &parameter : parameters) {
        if (!parameter.isValid() || !parameter.isEnabled())
            continue;

        switch (parameter.scope()) {
        case ApiRequestParameter::UrlPathScope:
            urlPath.replace('{' + parameter.name() + '}', parameter.value().toString());
            break;

        case ApiRequestParameter::UrlQueryScope:
            urlQuery.addQueryItem(parameter.name(), parameter.value().toString());
            break;

        case ApiRequestParameter::HeaderScope:
            netReq.setRawHeader(parameter.name().toLatin1(), parameter.value().toByteArray());
            break;
        }
    }

    url.setPath(urlPath);
    url.setQuery(urlQuery);

    netReq.setUrl(url);
    return netReq;
}

ApiBasePrivate::ApiBasePrivate(ApiBase *q) :
    q_ptr(q),
    m_netMan(nullptr)
{
}

QNetworkAccessManager *ApiBasePrivate::netMan() const
{
    if (!m_netMan) {
        m_netMan = new QNetworkAccessManager(q_ptr);
        m_netMan->setRedirectPolicy(QNetworkRequest::SameOriginRedirectPolicy);
    }
    return m_netMan;
}

void ApiBasePrivate::setNetMan(QNetworkAccessManager *man)
{
    m_netMan = man;
}

QByteArray ApiBasePrivate::httpVerbFromOperation(int op)
{
    switch (op) {
    case ApiBase::GetOperation:
        return QByteArrayLiteral("GET");

    case ApiBase::PostOperation:
        return QByteArrayLiteral("POST");

    case ApiBase::PutOperation:
        return QByteArrayLiteral("PUT");

    case ApiBase::PatchOperation:
        return QByteArrayLiteral("PATCH");

    case ApiBase::DeleteOperation:
        return QByteArrayLiteral("DELETE");

    default:
        return QByteArray();
    }
}

}
