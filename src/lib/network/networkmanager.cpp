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

#include <RestLink/networkresponse.h>
#include <RestLink/private/networkresponse_p.h>

#include <RestLink/private/emptyresponse_p.h>

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

QByteArray NetworkManager::handlerId() const
{
    return QByteArrayLiteral("restlink.network.manager");
}

Response *NetworkManager::sendRequest(Method method, const Request &request, const Body &body)
{
    const QString requestScheme = request.baseUrl().scheme();

    // If it's supported, send though QNetworkAccessManager base
    const QStringList networkSchemes = s_supportedNetworkSchemes;
    if (networkSchemes.contains(requestScheme)) {
        QNetworkRequest netRequest = generateNetworkRequest(method, request, body);
        QNetworkReply *netReply = generateNetworkReply(method, netRequest, body);

        NetworkResponse *response = new NetworkResponse(this);
        initResponse(response, request, method);
        response->setReply(netReply);
        return response;
    }

    // Otherwise, try using extra handlers
    QList<AbstractRequestHandler *> handlers = s_extraHandlers;

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

    EmptyResponse *response = new EmptyResponse(method, this);
    initResponse(response, request, method);
    response->netRequest = generateNetworkRequest(method, request, body);
    return response;
}

QStringList NetworkManager::supportedSchemes() const
{
    // If we don't have cached network schemes yet, we cache it now
    if (s_supportedNetworkSchemes.isEmpty())
        loadNetworkSchemes();

    // Merging and returning schemes
    return s_supportedNetworkSchemes + s_supportedHandlerSchemes;
}

AbstractRequestHandler::HandlerType NetworkManager::handlerType() const
{
    return HandlerType::NetworkManager;
}

NetworkManager::HandlerRegistrationError NetworkManager::registerHandler(AbstractRequestHandler *handler)
{
    // First, we check if the handler is valid (ie: has enough id informations)
    if (handler == nullptr || handler->handlerId().isEmpty())
        return InvalidHandlerRegistrationError;

    // Next, we check if it is not already registered
    auto it = std::find_if(s_extraHandlers.cbegin(), s_extraHandlers.cend(), [handler](const AbstractRequestHandler *current) {
        return handler == current || handler->handlerId() == current->handlerId();
    });

    if (it != s_extraHandlers.cend())
        return HandlerAlreadyRegisteredError;

    // If network schemes are not already available, we query them
    if (s_supportedNetworkSchemes.empty())
        loadNetworkSchemes();

    // Next, we check if its schemes are not already supported
    const QStringList oldSchemes = s_supportedNetworkSchemes + s_supportedHandlerSchemes;
    const QStringList newSchemes = handler->supportedSchemes();
    for (const QString &newScheme : newSchemes)
        if (oldSchemes.contains(newScheme))
            return HandlerSchemesAlreadyExistsError;

    // If everything ok, we process registration and report no errors
    s_supportedHandlerSchemes.append(handler->supportedSchemes());
    s_extraHandlers.append(handler);
    return NoHandlerRegistrationError;
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

void NetworkManager::loadNetworkSchemes()
{
    s_supportedNetworkSchemes = QNetworkAccessManager().supportedSchemes();

#ifdef Q_OS_WASM
    // Force HTTP/HTTPS on WASM
    s_supportedNetworkSchemes.append({ "https", "http" });
    s_supportedNetworkSchemes.removeDuplicates();
#endif
}

QStringList NetworkManager::s_supportedNetworkSchemes;
QStringList NetworkManager::s_supportedHandlerSchemes;
QVector<AbstractRequestHandler *> NetworkManager::s_extraHandlers;

}
