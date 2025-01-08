#include "networkmanager.h"

#include <RestLink/cache.h>
#include <RestLink/cookiejar.h>
#include <RestLink/request.h>
#include <RestLink/pathparameter.h>
#include <RestLink/queryparameter.h>
#include <RestLink/header.h>
#include <RestLink/body.h>
#include <RestLink/response.h>
#include <RestLink/private/networkresponse_p.h>
#include <RestLink/compressionutils.h>

#ifdef RESTLINK_SQL
#   include <RestLink/private/sqlhandler.h>
#endif

#include <QtCore/qcoreapplication.h>
#include <QtCore/qurl.h>
#include <QtCore/qurlquery.h>

#include <QtNetwork/qhttpmultipart.h>
#include <QtNetwork/qnetworkreply.h>

namespace RestLink {

/**
 * @class NetworkManager
 * @brief A custom network manager for handling HTTP requests and responses in RestLink.
 *
 * This class extends QNetworkAccessManager to provide additional functionality,
 * including custom caching and cookie management, as well as redirect policy control.
 * It is designed to be the core network handling component of RestLink.
 */

/**
 * @brief Constructs a NetworkManager with a specified parent.
 *
 * Initializes the network manager with a default cache and cookie jar.
 * Also sets the redirect policy to SameOriginRedirectPolicy to ensure
 * secure handling of redirections within the same origin.
 *
 * @param parent The parent object for this NetworkManager. Defaults to nullptr.
 */
NetworkManager::NetworkManager(QObject *parent)
    : QNetworkAccessManager{parent}
{
    setRedirectPolicy(QNetworkRequest::SameOriginRedirectPolicy);

    setCache(new Cache(this));
    setCookieJar(new CookieJar(this));

    initHandlers();
}

Response *NetworkManager::head(const Request &request, Api *api)
{
    return send(Api::HeadOperation, request, Body(), api);
}

Response *NetworkManager::get(const Request &request, Api *api)
{
    return send(Api::GetOperation, request, Body(), api);
}

Response *NetworkManager::post(const Request &request, const Body &body, Api *api)
{
    return send(Api::PostOperation, request, body, api);
}

Response *NetworkManager::put(const Request &request, const Body &body, Api *api)
{
    return send(Api::PutOperation, request, body, api);
}

Response *NetworkManager::patch(const Request &request, const Body &body, Api *api)
{
    return send(Api::PatchOperation, request, body, api);
}

Response *NetworkManager::deleteResource(const Request &request, Api *api)
{
    return send(Api::DeleteOperation, request, Body(), api);
}

Response *NetworkManager::send(Api::Operation operation, const Request &request, const Body &body, Api *api)
{
    const QString urlSheme = api->url().scheme();
    QVector<NetworkRequestHandler *>::Iterator it = std::find_if(s_handlers.begin(), s_handlers.end(), [urlSheme](const NetworkRequestHandler *handler) {
        return handler->schemes().contains(urlSheme);
    });

    Response *response;

    if (it != s_handlers.end()) {
        NetworkRequestHandler *handler = *it;
        handler->manager = this;
        response = handler->send(operation, request, body, api);
    } else {
        QNetworkRequest networkRequest = createNetworkRequest(operation, request, body, api);
        QNetworkReply *networkReply = createNetworkReply(operation, networkRequest, body);
        NetworkResponse *networkResponse = new NetworkResponse(api);
        networkResponse->setReply(networkReply);
        response = networkResponse;
    }

    response->setRequest(request);
    return response;
}

Response *NetworkManager::send(ApiBase::Operation operation, const Request &request, const Body &body, ApiBase *api)
{
    if (api->inherits("RestLink::Api"))
        return send(operation, request, body, static_cast<Api *>(api));
    else
        return nullptr;
}

QStringList NetworkManager::supportedSchemes() const
{
    // Getting schemes from QNetworkAccessManager and network handlers
    QStringList schemes = QNetworkAccessManager::supportedSchemes();
    for (NetworkRequestHandler *handler : s_handlers)
        schemes.append(schemes);

    // Removing duplicates and returning schemes
    schemes.removeDuplicates();
    return schemes;
}

QNetworkRequest NetworkManager::createNetworkRequest(ApiBase::Operation operation, const Request &request, const Body &body, ApiBase *api)
{
    QNetworkRequest netReq;
    netReq.setOriginatingObject(this);
    netReq.setAttribute(QNetworkRequest::UserMax, QVariant::fromValue(request));

    // Url generation
    QUrl url = api->url();
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
    netReq.setHeader(QNetworkRequest::UserAgentHeader, api->userAgent());

    // Accept headers setup

    netReq.setRawHeader("Accept", "*/*");

    {
        const QByteArrayList algorithms = CompressionUtils::supportedAlgorithms();
        if (!algorithms.isEmpty())
            netReq.setRawHeader("Accept-Encoding", algorithms.join(", "));
    }

    {
        const QString full = api->locale().name();
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

QNetworkReply *NetworkManager::createNetworkReply(ApiBase::Operation operation, const QNetworkRequest &request, const Body &body)
{
    QNetworkAccessManager *man = this;
    QNetworkReply *reply;

    switch (operation) {
    case Api::HeadOperation:
        reply = man->head(request);
        break;

    case Api::GetOperation:
        reply = man->get(request);
        break;

    case Api::PostOperation:
        if (body.isMultiPart())
            reply =  man->post(request, body.multiPart());
        else if (body.isDevice())
            reply =  man->post(request, body.device());
        else if (body.isData())
            reply =  man->post(request, body.data());
        else
            reply =  man->post(request, QByteArray());
        break;

    case Api::PutOperation:
        if (body.isMultiPart())
            reply =  man->put(request, body.multiPart());
        else if (body.isDevice())
            reply =  man->put(request, body.device());
        else if (body.isData())
            reply =  man->put(request, body.data());
        else
            reply =  man->put(request, QByteArray());
        break;

    case Api::PatchOperation:
        if (body.isMultiPart())
            reply =  man->sendCustomRequest(request, "PATCH", body.multiPart());
        else if (body.isDevice())
            reply =  man->sendCustomRequest(request, "PATCH", body.device());
        else if (body.isData())
            reply =  man->sendCustomRequest(request, "PATCH", body.data());
        else
            reply =  man->sendCustomRequest(request, "PATCH", QByteArray());
        break;

    case Api::DeleteOperation:
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

void NetworkManager::initHandlers()
{
    if (!s_handlers.isEmpty())
        return;

#ifdef RESTLINK_SQL
    s_handlers.append(new SqlHandler(qApp));
#endif
}

QVector<class NetworkRequestHandler *> NetworkManager::s_handlers;

QNetworkRequest NetworkRequestHandler::createNetworkRequest(ApiBase::Operation operation, const Request &request, const Body &body, ApiBase *api)
{
    return manager->createNetworkRequest(operation, request, body, api);
}

}
