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

/**
 * @class ApiBase
 * @brief The ApiBase class provides methods for making HTTP requests (HEAD, GET, POST, PUT, PATCH, DELETE) and handling responses.
 *
 * This class encapsulates the functionality for interacting with an API by providing methods for various HTTP operations.
 * It manages request interceptors, constructs network requests, and handles the creation of network replies and responses.
 * The class supports asynchronous communication, allowing the user to pass callbacks for handling responses.
 *
 * @note This class is must not be used directly, use Api class instead.
 *
 * @see Request, Response, RequestInterceptor
 */

ApiBase::ApiBase(ApiBasePrivate *d, QObject *parent) :
    QObject(parent),
    d_ptr(d)
{
}

/**
 * @brief Destructor for ApiBase.
 *
 * Deletes all request interceptors that are managed by ApiBase.
 */
ApiBase::~ApiBase()
{
    for (RequestInterceptor *interceptor : d_ptr->requestInterceptors)
        delete interceptor;
}

/**
 * @brief Returns the api locale.
 *
 * @return QLocale representing the api locale.
 *
 * @note This locale is used to set ACCEPT-LANGUAGE header only, if you want to pass it to the API you are using,
 * you must do it manually according to the used API documentation.
 */
QLocale ApiBase::locale() const
{
    return QLocale::system();
}

/**
 * @brief Makes a HEAD request and calls the provided callback upon completion.
 *
 * @param request The Request object to be sent.
 * @param callback The callback function to be invoked once the request completes.
 */
void ApiBase::head(const Request &request, std::function<ApiRunCallback> callback)
{
    Response *response = head(request);
    connect(response, &Response::finished, this, [callback, response] { callback(response); });
    connect(response, &Response::finished, response, &QObject::deleteLater);
}

/**
 * @brief Makes a HEAD request and returns the Response object.
 *
 * @param request The Request object to be sent.
 * @return A Response object containing the result of the HEAD request.
 */
Response *ApiBase::head(const Request &request)
{
    const Api::Operation operation = Api::HeadOperation;
    const Body body;
    const QNetworkRequest netRequest = createNetworkRequest(request, body, operation);
    QNetworkReply *netReply = createNetworkReply(netRequest, body, operation);
    return createResponse(request, netReply);
}

/**
 * @brief Makes a GET request and calls the provided callback upon completion.
 *
 * @param request The Request object to be sent.
 * @param callback The callback function to be invoked once the request completes.
 */
void ApiBase::get(const Request &request, std::function<ApiRunCallback> callback)
{
    Response *response = get(request);
    connect(response, &Response::finished, this, [callback, response] { callback(response); });
    connect(response, &Response::finished, response, &QObject::deleteLater);
}

/**
 * @brief Makes a GET request and returns the Response object.
 *
 * @param request The Request object to be sent.
 * @return A Response object containing the result of the GET request.
 */
Response *ApiBase::get(const Request &request)
{
    const Api::Operation operation = Api::GetOperation;
    const Body body;
    const QNetworkRequest netRequest = createNetworkRequest(request, body, operation);
    QNetworkReply *netReply = createNetworkReply(netRequest, body, operation);
    return createResponse(request, netReply);
}

/**
 * @brief Makes a POST request with a body and calls the provided callback upon completion.
 *
 * @param request The Request object to be sent.
 * @param body The Body object containing the data to be sent in the request.
 * @param callback The callback function to be invoked once the request completes.
 */
void ApiBase::post(const Request &request, const Body &body, std::function<ApiRunCallback> callback)
{
    Response *response = post(request, body);
    connect(response, &Response::finished, this, [callback, response] { callback(response); });
    connect(response, &Response::finished, response, &QObject::deleteLater);
}

/**
 * @brief Makes a POST request with a body and returns the Response object.
 *
 * @param request The Request object to be sent.
 * @param body The Body object containing the data to be sent in the request.
 * @return A Response object containing the result of the POST request.
 */
Response *ApiBase::post(const Request &request, const Body &body)
{
    const Api::Operation operation = Api::PostOperation;
    const QNetworkRequest netRequest = createNetworkRequest(request, body, operation);
    QNetworkReply *netReply = createNetworkReply(netRequest, body, operation);
    return createResponse(request, netReply);
}

/**
 * @brief Makes a PUT request with a body and calls the provided callback upon completion.
 *
 * @param request The Request object to be sent.
 * @param body The Body object containing the data to be sent in the request.
 * @param callback The callback function to be invoked once the request completes.
 */
void ApiBase::put(const Request &request, const Body &body, std::function<ApiRunCallback> callback)
{
    Response *response = put(request, body);
    connect(response, &Response::finished, this, [callback, response] { callback(response); });
    connect(response, &Response::finished, response, &QObject::deleteLater);
}

/**
 * @brief Makes a PUT request with a body and returns the Response object.
 *
 * @param request The Request object to be sent.
 * @param body The Body object containing the data to be sent in the request.
 * @return A Response object containing the result of the PUT request.
 */
Response *ApiBase::put(const Request &request, const Body &body)
{
    const Api::Operation operation = Api::PutOperation;
    const QNetworkRequest netRequest = createNetworkRequest(request, body, operation);
    QNetworkReply *netReply = createNetworkReply(netRequest, body, operation);
    return createResponse(request, netReply);
}

/**
 * @brief Makes a PATCH request with a body and calls the provided callback upon completion.
 *
 * @param request The Request object to be sent.
 * @param body The Body object containing the data to be sent in the request.
 * @param callback The callback function to be invoked once the request completes.
 */
void ApiBase::patch(const Request &request, const Body &body, std::function<ApiRunCallback> callback)
{
    Response *response = patch(request, body);
    connect(response, &Response::finished, this, [callback, response] { callback(response); });
    connect(response, &Response::finished, response, &QObject::deleteLater);
}

/**
 * @brief Makes a PATCH request with a body and returns the Response object.
 *
 * @param request The Request object to be sent.
 * @param body The Body object containing the data to be sent in the request.
 * @return A Response object containing the result of the PATCH request.
 */
Response *ApiBase::patch(const Request &request, const Body &body)
{
    const Api::Operation operation = Api::PatchOperation;
    const QNetworkRequest netRequest = createNetworkRequest(request, body, operation);
    QNetworkReply *netReply = createNetworkReply(netRequest, body, operation);
    return createResponse(request, netReply);
}

/**
 * @brief Makes a DELETE request and calls the provided callback upon completion.
 *
 * @param request The Request object to be sent.
 * @param callback The callback function to be invoked once the request completes.
 */
void ApiBase::deleteResource(const Request &request, std::function<ApiRunCallback> callback)
{
    Response *response = deleteResource(request);
    connect(response, &Response::finished, this, [callback, response] { callback(response); });
    connect(response, &Response::finished, response, &QObject::deleteLater);
}

/**
 * @brief Makes a DELETE request and returns the Response object.
 *
 * @param request The Request object to be sent.
 * @return A Response object containing the result of the DELETE request.
 */
Response *ApiBase::deleteResource(const Request &request)
{
    const Api::Operation operation = Api::DeleteOperation;
    const Body body;
    const QNetworkRequest netRequest = createNetworkRequest(request, body, operation);
    QNetworkReply *netReply = createNetworkReply(netRequest, body, operation);
    return createResponse(request, netReply);
}

/**
 * @brief Returns the user agent string for the API.
 *
 * @return A string representing the user agent.
 */
QString ApiBase::userAgent() const
{
    return QStringLiteral("libRestLink/") + QStringLiteral(RESTLINK_VERSION_STR);
}

/**
 * @brief Returns the list of request interceptors.
 *
 * @return A QList of request interceptors.
 */
QList<RequestInterceptor *> ApiBase::requestInterceptors() const
{
    return d_ptr->requestInterceptors.toList();
}

/**
 * @brief Adds a request interceptor to the list.
 *
 * @param interceptor The interceptor to be added.
 */
void ApiBase::addRequestInterceptor(RequestInterceptor *interceptor)
{
    if (!d_ptr->requestInterceptors.contains(interceptor))
        d_ptr->requestInterceptors.append(interceptor);
}

/**
 * @brief Removes a request interceptor from the list.
 *
 * @param interceptor The interceptor to be removed.
 */
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

/**
 * @brief Returns the network manager used by the ApiBase class.
 *
 * This function retrieves the current instance of the QNetworkAccessManager that is used for making network requests.
 *
 * @return A pointer to the QNetworkAccessManager instance.
 */
QNetworkAccessManager *ApiBase::networkManager() const
{
    return d_ptr->netMan();
}

/**
 * @brief Sets the network manager for the ApiBase class.
 *
 * This function allows setting a custom QNetworkAccessManager instance to be used for network requests.
 *
 * @param manager A pointer to the QNetworkAccessManager instance to be set.
 */
void ApiBase::setNetworkManager(QNetworkAccessManager *manager)
{
    d_ptr->setNetMan(manager);
}

/**
 * @brief Creates a network request for the given parameters.
 *
 * This function constructs a QNetworkRequest using the provided Request, Body, and Operation parameters. It merges the provided request with internal settings, applies any request interceptors, constructs the URL with query parameters, and sets necessary headers (such as User-Agent, Accept-Encoding, and Accept-Language). It also configures cache and connection settings before returning the fully-constructed network request.
 *
 * @param req The request object containing information like the endpoint and headers.
 * @param body The body of the request that contains any data or file to be sent.
 * @param operation The operation type (e.g., GET, POST, PUT) that specifies the HTTP method.
 *
 * @return The fully-constructed QNetworkRequest to be used in the network request.
 */
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

/**
 * @brief Creates a network reply based on the given request, body, and operation.
 *
 * This function uses the provided QNetworkRequest, Body, and Operation to determine the appropriate network reply to perform the specified HTTP operation. It handles various types of body content (e.g., multipart, device, data) and returns a QNetworkReply that corresponds to the operation being executed.
 *
 * @param request The QNetworkRequest that defines the network request to be made.
 * @param body The body of the request, which may contain data, files, or other content.
 * @param operation The HTTP operation to be performed (e.g., GET, POST, PUT, PATCH, DELETE).
 *
 * @return A pointer to the QNetworkReply object representing the network reply for the operation.
 */
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
