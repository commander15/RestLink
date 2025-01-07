#include "apibase.h"
#include "apibase_p.h"

#include <RestLink/debug.h>
#include <RestLink/request.h>
#include <RestLink/pathparameter.h>
#include <RestLink/queryparameter.h>
#include <RestLink/header.h>
#include <RestLink/body.h>
#include <RestLink/response.h>
#include <RestLink/requestinterceptor.h>
#include <RestLink/compressionutils.h>
#include <RestLink/networkmanager.h>

#include <RestLink/private/request_p.h>

#include <QtNetwork/qnetworkproxy.h>
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qhttpmultipart.h>

#include <QtCore/qurl.h>
#include <QtCore/qurlquery.h>

namespace RestLink {

ApiBase::ApiBase(ApiBasePrivate *d, QObject *parent) :
    QObject(parent),
    d_ptr(d)
{
}

ApiBase::~ApiBase()
{
    for (RequestInterceptor *interceptor : d_ptr->requestInterceptors)
        delete interceptor;
}

QLocale ApiBase::locale() const
{
    return QLocale::system();
}

void ApiBase::head(const Request &request, std::function<ApiRunCallback> callback)
{
    Response *response = head(request);
    connect(response, &Response::finished, this, [callback, response] { callback(response); });
    connect(response, &Response::finished, response, &QObject::deleteLater);
}

Response *ApiBase::head(const Request &request)
{
    const Api::Operation operation = Api::HeadOperation;
    const Body body;
    const QNetworkRequest netRequest = createNetworkRequest(request, body, operation);
    QNetworkReply *netReply = createNetworkReply(netRequest, body, operation);
    return createResponse(request, netReply);
}

void ApiBase::get(const Request &request, std::function<ApiRunCallback> callback)
{
    Response *response = get(request);
    connect(response, &Response::finished, this, [callback, response] { callback(response); });
    connect(response, &Response::finished, response, &QObject::deleteLater);
}

Response *ApiBase::get(const Request &request)
{
    const Api::Operation operation = Api::GetOperation;
    const Body body;
    const QNetworkRequest netRequest = createNetworkRequest(request, body, operation);
    QNetworkReply *netReply = createNetworkReply(netRequest, body, operation);
    return createResponse(request, netReply);
}

void ApiBase::post(const Request &request, const Body &body, std::function<ApiRunCallback> callback)
{
    Response *response = post(request, body);
    connect(response, &Response::finished, this, [callback, response] { callback(response); });
    connect(response, &Response::finished, response, &QObject::deleteLater);
}

Response *ApiBase::post(const Request &request, const Body &body)
{
    const Api::Operation operation = Api::PostOperation;
    const QNetworkRequest netRequest = createNetworkRequest(request, body, operation);
    QNetworkReply *netReply = createNetworkReply(netRequest, body, operation);
    return createResponse(request, netReply);
}

void ApiBase::put(const Request &request, const Body &body, std::function<ApiRunCallback> callback)
{
    Response *response = put(request, body);
    connect(response, &Response::finished, this, [callback, response] { callback(response); });
    connect(response, &Response::finished, response, &QObject::deleteLater);
}

Response *ApiBase::put(const Request &request, const Body &body)
{
    const Api::Operation operation = Api::PutOperation;
    const QNetworkRequest netRequest = createNetworkRequest(request, body, operation);
    QNetworkReply *netReply = createNetworkReply(netRequest, body, operation);
    return createResponse(request, netReply);
}

void ApiBase::patch(const Request &request, const Body &body, std::function<ApiRunCallback> callback)
{
    Response *response = patch(request, body);
    connect(response, &Response::finished, this, [callback, response] { callback(response); });
    connect(response, &Response::finished, response, &QObject::deleteLater);
}

Response *ApiBase::patch(const Request &request, const Body &body)
{
    const Api::Operation operation = Api::PatchOperation;
    const QNetworkRequest netRequest = createNetworkRequest(request, body, operation);
    QNetworkReply *netReply = createNetworkReply(netRequest, body, operation);
    return createResponse(request, netReply);
}

void ApiBase::deleteResource(const Request &request, std::function<ApiRunCallback> callback)
{
    Response *response = deleteResource(request);
    connect(response, &Response::finished, this, [callback, response] { callback(response); });
    connect(response, &Response::finished, response, &QObject::deleteLater);
}

Response *ApiBase::deleteResource(const Request &request)
{
    const Api::Operation operation = Api::DeleteOperation;
    const Body body;
    const QNetworkRequest netRequest = createNetworkRequest(request, body, operation);
    QNetworkReply *netReply = createNetworkReply(netRequest, body, operation);
    return createResponse(request, netReply);
}

QString ApiBase::userAgent() const
{
    return QStringLiteral("libRestLink/") + QStringLiteral(RESTLINK_VERSION_STR);
}

QList<RequestInterceptor *> ApiBase::requestInterceptors() const
{
    return d_ptr->requestInterceptors.toList();
}

void ApiBase::addRequestInterceptor(RequestInterceptor *interceptor)
{
    if (!d_ptr->requestInterceptors.contains(interceptor))
        d_ptr->requestInterceptors.append(interceptor);
}

void ApiBase::removeRequestInterceptor(RequestInterceptor *interceptor)
{
    d_ptr->requestInterceptors.removeOne(interceptor);
}

QAbstractNetworkCache *ApiBase::cache() const
{
    return d_ptr->netMan()->cache();
}

void ApiBase::setCache(QAbstractNetworkCache *cache)
{
    d_ptr->netMan()->setCache(cache);
}

QNetworkCookieJar *ApiBase::cookieJar() const
{
    return d_ptr->netMan()->cookieJar();
}

void ApiBase::setCookieJar(QNetworkCookieJar *jar)
{
    d_ptr->netMan()->setCookieJar(jar);
}

QNetworkProxy ApiBase::proxy() const
{
    return d_ptr->netMan()->proxy();
}

void ApiBase::setProxy(const QNetworkProxy &proxy)
{
    d_ptr->netMan()->setProxy(proxy);
}

QNetworkAccessManager *ApiBase::networkManager() const
{
    return d_ptr->netMan();
}

void ApiBase::setNetworkManager(QNetworkAccessManager *manager)
{
    d_ptr->setNetMan(manager);
}

QNetworkRequest ApiBase::createNetworkRequest(const Request &req, const Body &body, Operation operation)
{
    // Creating the final request
    Request request = Request::merge(req, d_ptr->internalRequest);
    if (!req.endpoint().isEmpty())
        for (RequestInterceptor *interceptor : d_ptr->requestInterceptors)
            request = interceptor->intercept(request, body, operation);

    QNetworkRequest netReq;
    netReq.setOriginatingObject(this);
    netReq.setAttribute(QNetworkRequest::UserMax, QVariant::fromValue(request));

    // Url generation
    QUrl url = this->url();
    {
        QString urlPath = url.path() + request.urlPath();
        url.setPath(urlPath);

        QUrlQuery urlQuery(url.query());
        for (const QueryParameter &param : request.queryParameters())
            if (param.isEnabled())
                for (const QVariant &value : param.values())
                    urlQuery.addQueryItem(param.name(), value.toString());
        url.setQuery(urlQuery);
    }
    netReq.setUrl(url);


    // User agent setting
    netReq.setHeader(QNetworkRequest::UserAgentHeader, userAgent());

    // Accept headers setup

    netReq.setRawHeader("Accept", "*/*");

    {
        const QByteArrayList algorithms = CompressionUtils::supportedAlgorithms();
        if (!algorithms.isEmpty())
            netReq.setRawHeader("Accept-Encoding", algorithms.join(", "));
    }

    {
        const QString full = locale().name();
        const QString slim = full.section('_', 0, 0);
        netReq.setRawHeader("Accept-Language", QStringLiteral("%1,%2;q=0.5").arg(full, slim).toLatin1());
    }

    // Make keep alive
    netReq.setRawHeader("Connection", "keep-alive");

    // Cache settings
    if (true) {
        netReq.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        netReq.setAttribute(QNetworkRequest::CacheSaveControlAttribute, true);
    } else {
        netReq.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
        netReq.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);
    }

    // Request headers setup
    for (const Header &header : request.headers() + body.headers()) {
        if (header.isEnabled()) {
            const QVariantList values = header.values();
            QList<QByteArray> rawValues;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            rawValues.reserve(values.size());
            std::transform(values.begin(), values.end(), std::back_inserter(rawValues), [](const QVariant &value) {
                return value.toByteArray();
            });
#else
            for (const QVariant &value : values)
                rawValues.append(value.toByteArray());
#endif
            netReq.setRawHeader(header.name().toUtf8(), rawValues.join(','));
        }
    }

    return netReq;
}

QNetworkReply *ApiBase::createNetworkReply(const QNetworkRequest &request, const Body &body, Operation operation)
{
    QNetworkAccessManager *man = d_ptr->netMan();
    QNetworkReply *reply;

    switch (operation) {
    case HeadOperation:
        reply = man->head(request);
        break;

    case GetOperation:
        reply = man->get(request);
        break;

    case PostOperation:
        if (body.isMultiPart())
            reply =  man->post(request, body.multiPart());
        else if (body.isDevice())
            reply =  man->post(request, body.device());
        else if (body.isData())
            reply =  man->post(request, body.data());
        else
            reply =  man->post(request, QByteArray());
        break;

    case PutOperation:
        if (body.isMultiPart())
            reply =  man->put(request, body.multiPart());
        else if (body.isDevice())
            reply =  man->put(request, body.device());
        else if (body.isData())
            reply =  man->put(request, body.data());
        else
            reply =  man->put(request, QByteArray());
        break;

    case PatchOperation:
        if (body.isMultiPart())
            reply =  man->sendCustomRequest(request, "PATCH", body.multiPart());
        else if (body.isDevice())
            reply =  man->sendCustomRequest(request, "PATCH", body.device());
        else if (body.isData())
            reply =  man->sendCustomRequest(request, "PATCH", body.data());
        else
            reply =  man->sendCustomRequest(request, "PATCH", QByteArray());
        break;

    case DeleteOperation:
        reply =  man->deleteResource(request);
        break;

    default:
        reply =  nullptr;
    }

    if (body.isMultiPart()) {
        QHttpMultiPart *multiPart = body.multiPart();
        if (!multiPart->parent())
            multiPart->setParent(reply ? static_cast<QObject *>(reply) : static_cast<QObject *>(this));
    }

    if (body.isDevice()) {
        QIODevice *device = body.device();
        if (!device->parent())
            device->setParent(reply ? static_cast<QObject *>(reply) : static_cast<QObject *>(this));
    }

    return reply;
}

const QList<PathParameter> *ApiBase::constPathParameters() const
{
    return &d_ptr->internalRequest.d_ptr->pathParameters;
}

QList<PathParameter> *ApiBase::mutablePathParameters()
{
    return &d_ptr->internalRequest.d_ptr->pathParameters;
}

const QList<QueryParameter> *ApiBase::constQueryParameters() const
{
    return &d_ptr->internalRequest.d_ptr->queryParameters;
}

QList<QueryParameter> *ApiBase::mutableQueryParameters()
{
    return &d_ptr->internalRequest.d_ptr->queryParameters;
}

const QList<Header> *ApiBase::constHeaders() const
{
    return &d_ptr->internalRequest.d_ptr->headers;
}

QList<Header> *ApiBase::mutableHeaders()
{
    return &d_ptr->internalRequest.d_ptr->headers;
}

ApiBasePrivate::ApiBasePrivate(ApiBase *q) :
    q_ptr(q),
    m_netMan(nullptr)
{
}

QNetworkAccessManager *ApiBasePrivate::netMan() const
{
    if (!m_netMan)
        m_netMan = new NetworkManager(q_ptr);
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
