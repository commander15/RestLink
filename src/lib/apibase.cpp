#include "apibase.h"
#include "apibase_p.h"

#include <RestLink/debug.h>
#include <RestLink/request.h>
#include <RestLink/body.h>
#include <RestLink/response.h>
#include <RestLink/requestinterceptor.h>
#include <RestLink/networkmanager.h>
#include <RestLink/private/request_p.h>

namespace RestLink {

/**
 * @class ApiBase
 * @brief The ApiBase class provides methods for making HTTP requests (HEAD, GET, POST, PUT, PATCH, DELETE) and handling responses.
 *
 * This class encapsulates the functionality for interacting with an API by providing methods for various HTTP operations.
 * It manages request interceptors, constructs network requests, and handles the creation of network replies and responses.
 * The class supports asynchronous communication, allowing the user to pass callbacks for handling responses.
 *
 * @note This class must not be used directly, use Api class instead.
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
 */
ApiBase::~ApiBase()
{
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
void ApiBase::head(const Request &request, const ApiRunCallback &callback)
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
    return send(Api::HeadOperation, request, Body());
}

/**
 * @brief Makes a GET request and calls the provided callback upon completion.
 *
 * @param request The Request object to be sent.
 * @param callback The callback function to be invoked once the request completes.
 */
void ApiBase::get(const Request &request, const ApiRunCallback &callback)
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
    return send(GetOperation, request, Body());
}

/**
 * @brief Makes a POST request with a body and calls the provided callback upon completion.
 *
 * @param request The Request object to be sent.
 * @param body The Body object containing the data to be sent in the request.
 * @param callback The callback function to be invoked once the request completes.
 */
void ApiBase::post(const Request &request, const Body &body, const ApiRunCallback &callback)
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
    return send(PostOperation, request, body);
}

/**
 * @brief Makes a PUT request with a body and calls the provided callback upon completion.
 *
 * @param request The Request object to be sent.
 * @param body The Body object containing the data to be sent in the request.
 * @param callback The callback function to be invoked once the request completes.
 */
void ApiBase::put(const Request &request, const Body &body, const ApiRunCallback &callback)
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
    return send(PutOperation, request, body);
}

/**
 * @brief Makes a PATCH request with a body and calls the provided callback upon completion.
 *
 * @param request The Request object to be sent.
 * @param body The Body object containing the data to be sent in the request.
 * @param callback The callback function to be invoked once the request completes.
 */
void ApiBase::patch(const Request &request, const Body &body, const ApiRunCallback &callback)
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
    return send(PatchOperation, request, body);
}

/**
 * @brief Makes a DELETE request and calls the provided callback upon completion.
 *
 * @param request The Request object to be sent.
 * @param callback The callback function to be invoked once the request completes.
 */
void ApiBase::deleteResource(const Request &request, const ApiRunCallback &callback)
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
    return send(DeleteOperation, request, Body());
}

Response *ApiBase::send(Operation operation, const Request &request, const Body &body)
{
    // Preprocessing request by adding api url parameters and headers
    Request finalRequest = request;
    finalRequest.setApi(d_ptr->internalRequestData->api);

    // Preprocessing request by passing it to interceptors
    for (RequestInterceptor *interceptor : std::as_const(d_ptr->requestInterceptors))
        finalRequest = interceptor->intercept(finalRequest, body, operation);

    // Sending request and return response
    return d_ptr->networkManager()->send(operation, finalRequest, body);
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

/**
 * @brief Returns the network manager used by the ApiBase class.
 *
 * This function retrieves the current instance of the QNetworkAccessManager that is used for making network requests.
 *
 * @return A pointer to the QNetworkAccessManager instance.
 */
NetworkManager *ApiBase::networkManager() const
{
    return d_ptr->networkManager();
}

/**
 * @brief Sets the network manager for the ApiBase class.
 *
 * This function allows setting a custom QNetworkAccessManager instance to be used for network requests.
 *
 * @param manager A pointer to the QNetworkAccessManager instance to be set.
 */
void ApiBase::setNetworkManager(NetworkManager *manager)
{
    d_ptr->setNetworkManager(manager);
}

const QList<PathParameter> *ApiBase::constPathParameters() const
{
    return &d_ptr->internalRequestData->pathParameters;
}

QList<PathParameter> *ApiBase::mutablePathParameters()
{
    return &d_ptr->internalRequestData->pathParameters;
}

const QList<QueryParameter> *ApiBase::constQueryParameters() const
{
    return &d_ptr->internalRequestData->queryParameters;
}

QList<QueryParameter> *ApiBase::mutableQueryParameters()
{
    return &d_ptr->internalRequestData->queryParameters;
}

const QList<Header> *ApiBase::constHeaders() const
{
    return &d_ptr->internalRequestData->headers;
}

QList<Header> *ApiBase::mutableHeaders()
{
    return &d_ptr->internalRequestData->headers;
}

ApiBasePrivate::ApiBasePrivate(ApiBase *q)
    : q_ptr(q)
    , internalRequestData(new RequestData())
    , m_networkManager(nullptr)
{
    internalRequestData->ref.ref();
}

ApiBasePrivate::~ApiBasePrivate()
{
    if (!internalRequestData->ref.deref())
        delete internalRequestData;
}

NetworkManager *ApiBasePrivate::networkManager() const
{
    if (!m_networkManager)
        m_networkManager = new NetworkManager(q_ptr);
    return m_networkManager;
}

void ApiBasePrivate::setNetworkManager(NetworkManager *manager)
{
    m_networkManager = manager;
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
