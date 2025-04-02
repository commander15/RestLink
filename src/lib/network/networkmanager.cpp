#include "networkmanager.h"

#include <RestLink/debug.h>
#include <RestLink/request.h>
#include <RestLink/pathparameter.h>
#include <RestLink/queryparameter.h>
#include <RestLink/header.h>
#include <RestLink/body.h>
#include <RestLink/response.h>
#include <RestLink/private/networkresponse_p.h>
#include <RestLink/httputils.h>
#include <RestLink/compressionutils.h>
#include <RestLink/plugin.h>

#include <QtCore/qcoreapplication.h>

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
}

Response *NetworkManager::sendRequest(Api::Operation operation, const Request &request, const Body &body)
{
    const QUrl url = request.url(Request::PublicUrl);
    restlinkInfo() << HttpUtils::verbString(operation) << ' ' << url.toString(QUrl::DecodeReserved);

    QList<RequestHandler *> handlers = Plugin::allHandlers();

    const QString urlSheme = url.scheme();
    auto it = std::find_if(handlers.begin(), handlers.end(), [urlSheme](const RequestHandler *handler) {
        return handler->supportedSchemes().contains(urlSheme);
    });

    Response *response;

    if (it != handlers.end()) {
        RequestHandler *handler = *it;
        response = handler->send(operation, request, body);
    } else {
        QNetworkRequest networkRequest = generateNetworkRequest(operation, request, body);
        QNetworkReply *networkReply = generateNetworkReply(operation, networkRequest, body);

        Api *api = request.api();
        response = new NetworkResponse(networkReply, api);

        if (!api)
            response->setParent(this);
    }

    return response;
}

QStringList NetworkManager::supportedSchemes() const
{
    // Getting schemes from QNetworkAccessManager and network handlers
    QStringList schemes = QNetworkAccessManager::supportedSchemes();
    const QList<RequestHandler *> handlers = Plugin::allHandlers();
    for (RequestHandler *handler : handlers)
        schemes.append(handler->supportedSchemes());

    // Removing duplicates and returning schemes
    schemes.removeDuplicates();
    return schemes;
}

RequestHandler::HandlerType NetworkManager::handlerType() const
{
    return RequestHandler::NetworkManager;
}

QNetworkRequest NetworkManager::generateNetworkRequest(ApiBase::Operation operation, const Request &request, const Body &body)
{
    Api *api = request.api();

    QNetworkRequest netReq;
    netReq.setOriginatingObject(api);
    netReq.setAttribute(QNetworkRequest::UserMax, QVariant::fromValue(request));

    // Url generation
    netReq.setUrl(request.url());

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
    const QList<Header> headers = request.headers() + body.headers();
    for (const Header &header : headers) {
        const QVariantList values = header.values();
        QByteArrayList rawValues;
        if (header.hasFlag(Parameter::Locale) && values.isEmpty()) {
            const QString language = api->locale().name();
            rawValues.append(language.section('_', 0, 0).toLatin1());
        } else {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            rawValues.reserve(values.size());
            std::transform(values.begin(), values.end(), std::back_inserter(rawValues), [](const QVariant &value) {
                return value.toByteArray();
            });
#else
            for (const QVariant &value : values)
                rawValues.append(value.toByteArray());
#endif
        }

        netReq.setRawHeader(header.name().toUtf8(), rawValues.join(','));
    }

    return netReq;
}

QNetworkReply *NetworkManager::generateNetworkReply(ApiBase::Operation operation, const QNetworkRequest &request, const Body &body)
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

}
