#include "networkmanager.h"

#include <RestLink/debug.h>
#include <RestLink/request.h>
#include <RestLink/pathparameter.h>
#include <RestLink/queryparameter.h>
#include <RestLink/header.h>
#include <RestLink/body.h>
#include <RestLink/response.h>
#include <RestLink/httputils.h>
#include <RestLink/compressionutils.h>
#include <RestLink/pluginmanager.h>

#include <RestLink/private/networkresponse_p.h>

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

    static bool firstLoad = true;
    if (firstLoad) {
        restlinkInfo() << "supported network schemes: " << QNetworkAccessManager::supportedSchemes().join(", ");
        firstLoad = false;
    }
}

Response *NetworkManager::sendRequest(Method method, const Request &request, const Body &body)
{
#ifdef RESTLINK_DEBUG
    restlinkInfo() << HttpUtils::verbString(method) << ' ' << request.url(Request::PublicUrl).toString(QUrl::DecodeReserved);
#endif

    static auto registerForLogging = [](Response *response) {
        QObject::connect(response, &Response::finished, response, [response] {
            if (response->isSuccess())
                return;

            if (response->hasHttpStatusCode())
                restlinkWarning() << "HTTP " << response->httpStatusCode() << ' ' << response->httpReasonPhrase();
            else if (response->hasNetworkError())
                restlinkWarning() << "Network error: " << response->networkErrorString();
            else
                restlinkWarning() << "Unkown error occured";
        });
    };

    const QString scheme = request.baseUrl().scheme();

    // If it's supported, send though QNetworkAccessManager base
    const QStringList networkSchemes = QNetworkAccessManager::supportedSchemes();
    if (networkSchemes.contains(scheme) || scheme.startsWith("http")) {
        QNetworkRequest netRequest = generateNetworkRequest(method, request, body);
        QNetworkReply *netReply = generateNetworkReply(method, netRequest, body);

        NetworkResponse *response = new NetworkResponse(this);
        initResponse(response, request, method);
        response->setReply(netReply);
        registerForLogging(response);
        return response;
    }

    // Otherwise, try using plugin handlers
    QList<RequestHandler *> handlers = PluginManager::handlers();

    auto it = std::find_if(handlers.begin(), handlers.end(), [&scheme](const RequestHandler *handler) {
        return handler->supportedSchemes().contains(scheme);
    });

    if (it != handlers.end()) {
        RequestHandler *handler = *it;

        Response *response = handler->send(method, request, body);
        if (!response)
            restlinkWarning() << handler->handlerName() << ": response object creation failed, probably plugin related error";
        else
            registerForLogging(response);
        return response;
    }

    // We don't known the scheme, we just go null ;)
    restlinkWarning() << "NetworkManager: unsupported scheme usage detected !";
    return nullptr;
}

QStringList NetworkManager::supportedSchemes() const
{
    // Getting schemes from QNetworkAccessManager and network handlers
    QStringList schemes = QNetworkAccessManager::supportedSchemes();
    const QList<RequestHandler *> handlers = PluginManager::handlers();
    for (RequestHandler *handler : handlers)
        schemes.append(handler->supportedSchemes());

    // Removing duplicates and returning schemes
    schemes.removeDuplicates();
    schemes.append("https");
    return schemes;
}

RequestHandler::HandlerType NetworkManager::handlerType() const
{
    return RequestHandler::NetworkManager;
}

QNetworkRequest NetworkManager::generateNetworkRequest(Method method, const Request &request, const Body &body)
{
    const QUrl url = request.url();
    QHttpHeaders headers = request.httpHeaders();

    // Body headers
    const HeaderList bodyHeaders = body.headers();
    for (const Header &header : bodyHeaders) {
        if (headers.contains(header.name()))
            headers.removeAll(header.name());

        const QVariantList values = header.values();
        for (const QVariant &value : values)
            headers.append(header.name(), value.toString());
    }

    // Compression support
    if (request.attribute(Request::CompressionAllowedAttribute, true).toBool() && !headers.contains(QHttpHeaders::WellKnownHeader::AcceptEncoding)) {
        const QByteArrayList algorithms = CompressionUtils::supportedAlgorithms();
        if (!algorithms.isEmpty())
            headers.append(QHttpHeaders::WellKnownHeader::AcceptEncoding, algorithms.join(", "));
    }

    auto fillGap = [&headers](QHttpHeaders::WellKnownHeader header, const QAnyStringView &value) {
        if (!headers.contains(header))
            headers.append(header, value);
    };

    // Make keep alive
    fillGap(QHttpHeaders::WellKnownHeader::Connection, "keep-alive");

    // Default Accept all kind of media types
    fillGap(QHttpHeaders::WellKnownHeader::Accept, "*/*");

    // User Agent
    fillGap(QHttpHeaders::WellKnownHeader::UserAgent, "libRestLink/" + QStringLiteral(RESTLINK_VERSION_STR));

    QNetworkRequest netRequest(url);
    netRequest.setOriginatingObject(request.api());
    netRequest.setHeaders(headers);
    netRequest.setAttribute(QNetworkRequest::UserMax, QVariant::fromValue(request));

    // Request attributes
    auto applyAttribute = [&request, &netRequest](Request::Attribute source, QNetworkRequest::Attribute target) {
        const QVariant value = request.attribute(source);
        if (value.isValid())
            netRequest.setAttribute(target, value);
    };

    // Cache settings
    applyAttribute(Request::CacheLoadControlAttribute, QNetworkRequest::CacheLoadControlAttribute);
    applyAttribute(Request::CacheSaveControlAttribute, QNetworkRequest::CacheSaveControlAttribute);

    return netRequest;
}

QNetworkReply *NetworkManager::generateNetworkReply(Method method, const QNetworkRequest &request, const Body &body)
{
    QNetworkAccessManager *man = this;
    QNetworkReply *reply;

    switch (method) {
    case RequestHandler::HeadMethod:
        reply = man->head(request);
        break;

    case RequestHandler::GetMethod:
        reply = man->get(request);
        break;

    case RequestHandler::PostMethod:
        if (body.isMultiPart())
            reply =  man->post(request, body.multiPart());
        else if (body.isDevice())
            reply =  man->post(request, body.device());
        else if (body.isData())
            reply =  man->post(request, body.data());
        else
            reply =  man->post(request, QByteArray());
        break;

    case RequestHandler::PutMethod:
        if (body.isMultiPart())
            reply =  man->put(request, body.multiPart());
        else if (body.isDevice())
            reply =  man->put(request, body.device());
        else if (body.isData())
            reply =  man->put(request, body.data());
        else
            reply =  man->put(request, QByteArray());
        break;

    case RequestHandler::PatchMethod:
        if (body.isMultiPart())
            reply =  man->sendCustomRequest(request, "PATCH", body.multiPart());
        else if (body.isDevice())
            reply =  man->sendCustomRequest(request, "PATCH", body.device());
        else if (body.isData())
            reply =  man->sendCustomRequest(request, "PATCH", body.data());
        else
            reply =  man->sendCustomRequest(request, "PATCH", QByteArray());
        break;

    case RequestHandler::DeleteMethod:
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
