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
}

Response *NetworkManager::sendRequest(Method method, const Request &request, const Body &body)
{
    const QString requestScheme = request.baseUrl().scheme();
#ifdef Q_OS_WASM
    // Workaround for a Qt bug on WebAssembly, http and https didn't appears in supported schemes
    const QStringList httpSchemes = { "http", "https" }; // force HTTP/HTTPS support on WASM
#else
    // Here we don't enforce https cause it depends on SSL support and works well on non WASM platform
    const QStringList httpSchemes = { "http" }; // force HTTP support on all platforms
#endif

    // If it's supported, send though QNetworkAccessManager base
    const QStringList networkSchemes = QNetworkAccessManager::supportedSchemes();
    if (httpSchemes.contains(requestScheme) || networkSchemes.contains(requestScheme)) {
        QNetworkRequest netRequest = generateNetworkRequest(method, request, body);
        QNetworkReply *netReply = generateNetworkReply(method, netRequest, body);

        NetworkResponse *response = new NetworkResponse(this);
        initResponse(response, request, method);
        response->setReply(netReply);
        return response;
    }

    // Otherwise, try using plugin handlers
    QList<AbstractRequestHandler *> handlers = PluginManager::handlers();

    auto it = std::find_if(handlers.begin(), handlers.end(), [&requestScheme](const AbstractRequestHandler *handler) {
        return handler->supportedSchemes().contains(requestScheme);
    });

    if (it != handlers.end()) {
        AbstractRequestHandler *handler = *it;

        Response *response = handler->send(method, request, body);
        if (!response)
            restlinkWarning() << handler->handlerName() << ": response object creation failed, probably plugin related error";
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
    const QList<AbstractRequestHandler *> handlers = PluginManager::handlers();
    for (AbstractRequestHandler *handler : handlers)
        schemes.append(handler->supportedSchemes());

#ifdef Q_OS_WASM
    // Force HTTP/HTTPS on WASM
    schemes.append({ "https", "http" });
#endif

    // Removing duplicates and returning schemes
    schemes.removeDuplicates();
    return schemes;
}

AbstractRequestHandler::HandlerType NetworkManager::handlerType() const
{
    return AbstractRequestHandler::NetworkManager;
}

QNetworkRequest NetworkManager::generateNetworkRequest(Method method, const Request &request, const Body &body)
{
    const QUrl url = request.url();
    QHttpHeaders httpHeaders;

    // Api, Request and Body headers
    const HeaderList allHeaders = (request.api() ? request.api()->headers() : HeaderList()) + request.headers() + body.headers();
    for (const Header &header : allHeaders) {
        const QString name = header.name();
        if (httpHeaders.contains(name))
            httpHeaders.removeAll(name);

        const QVariantList values = header.values();
        for (const QVariant &value : values)
            httpHeaders.append(name, value.toString());
    }

    // Compression support
    if (request.attribute(Request::CompressionAllowedAttribute, true).toBool() && !httpHeaders.contains(QHttpHeaders::WellKnownHeader::AcceptEncoding)) {
        const QByteArrayList algorithms = CompressionUtils::supportedAlgorithms();
        if (!algorithms.isEmpty())
            httpHeaders.append(QHttpHeaders::WellKnownHeader::AcceptEncoding, algorithms.join(", "));
    }

    auto fillGap = [&httpHeaders](QHttpHeaders::WellKnownHeader header, const QAnyStringView &value) {
        if (!httpHeaders.contains(header))
            httpHeaders.append(header, value);
    };

    // Make keep alive
    fillGap(QHttpHeaders::WellKnownHeader::Connection, "keep-alive");

    // Default Accept all kind of media types
    fillGap(QHttpHeaders::WellKnownHeader::Accept, "*/*");

    // User Agent
    fillGap(QHttpHeaders::WellKnownHeader::UserAgent, "libRestLink/" + QStringLiteral(RESTLINK_VERSION_STR));

    QNetworkRequest netRequest(url);
    netRequest.setOriginatingObject(request.api());
    netRequest.setHeaders(httpHeaders);
    netRequest.setAttribute(QNetworkRequest::UserMax, QVariant::fromValue(request));

    auto contentType = httpHeaders.values(QHttpHeaders::WellKnownHeader::ContentType);
    Q_UNUSED(contentType);

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
    case AbstractRequestHandler::HeadMethod:
        reply = man->head(request);
        break;

    case AbstractRequestHandler::GetMethod:
        reply = man->get(request);
        break;

    case AbstractRequestHandler::PostMethod:
        if (body.isMultiPart())
            reply =  man->post(request, body.multiPart());
        else if (body.isDevice())
            reply =  man->post(request, body.device());
        else
            reply =  man->post(request, body.toByteArray());
        break;

    case AbstractRequestHandler::PutMethod:
        if (body.isMultiPart())
            reply =  man->put(request, body.multiPart());
        else if (body.isDevice())
            reply =  man->put(request, body.device());
        else
            reply =  man->put(request, body.toByteArray());
        break;

    case AbstractRequestHandler::PatchMethod:
        if (body.isMultiPart())
            reply =  man->sendCustomRequest(request, "PATCH", body.multiPart());
        else if (body.isDevice())
            reply =  man->sendCustomRequest(request, "PATCH", body.device());
        else
            reply =  man->sendCustomRequest(request, "PATCH", body.toByteArray());
        break;

    case AbstractRequestHandler::DeleteMethod:
        reply =  man->deleteResource(request);
        break;

    default:
        reply =  nullptr;
    }

    QObject *child;
    if (body.isMultiPart())
        child = body.multiPart();
    else if (body.isDevice())
        child = body.device();
    else
        child = nullptr;

    if (child)
        child->setParent(reply ? static_cast<QObject *>(reply) : static_cast<QObject *>(this));

    return reply;
}

}
