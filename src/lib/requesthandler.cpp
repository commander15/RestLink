#include "requesthandler.h"
#include "requesthandler_p.h"

#include <RestLink/debug.h>
#include <RestLink/request.h>
#include <RestLink/body.h>
#include <RestLink/response.h>
#include <RestLink/requestinterceptor.h>
#include <RestLink/httputils.h>

namespace RestLink {

/*!
 * \class RestLink::RequestHandler
 * \brief The RequestHandler class defines an abstract base for sending HTTP-like requests.
 *
 * It provides generic methods to send requests using standard HTTP methods (GET, POST, etc.).
 * Subclasses must implement scheme support and request sending behavior.
 *
 * \see NetworkManager
 */

/*!
 * \enum RequestHandler::Method
 * \brief Defines supported HTTP methods for handling outgoing requests.
 *
 * This enum is used to identify the HTTP method associated with a request.
 *
 * \var RequestHandler::Method RequestHandler::HeadMethod
 * Represents the HTTP HEAD method, used to retrieve headers without the response body.
 * \var RequestHandler::Method RequestHandler::GetMethod
 * Represents the HTTP GET method, used to retrieve data from the server.
 * \var RequestHandler::Method RequestHandler::PostMethod
 * Represents the HTTP POST method, used to submit data to the server.
 * \var RequestHandler::Method RequestHandler::PutMethod
 * Represents the HTTP PUT method, used to update or replace existing resources.
 * \var RequestHandler::Method RequestHandler::PatchMethod
 * Represents the HTTP PATCH method, used to apply partial modifications to a resource.
 * \var RequestHandler::Method RequestHandler::DeleteMethod
 * Represents the HTTP DELETE method, used to remove a resource.
 * \var RequestHandler::Method RequestHandler::UnknownMethod
 * Represents an unknown or unsupported HTTP method.
 */

/*!
 * \enum RequestHandler::HandlerType
 * \brief Describes the type of request handler being used.
 *
 * This enum helps identify the underlying implementation responsible for processing the request.

 * \var RequestHandler::HandlerType RequestHandler::NetworkManager
 * Indicates that the handler is based on the NetworkManager class (typically used for client-side requests).
 * \var RequestHandler::HandlerType RequestHandler::ServerHandler
 * Indicates that the handler is based on the Server class (typically used for server-side processing).
 * \var RequestHandler::HandlerType RequestHandler::UnknownHandler
 * Indicates an unknown or custom handler type not explicitly defined by the framework.
 */

/*!
 * \brief Destructor for RequestHandler.
 */
RequestHandler::~RequestHandler()
{
}

/*!
 * \brief Returns the name of the handler based on its type.
 * \note this method can be overriden on subclasses.
 */
QString RequestHandler::handlerName() const
{
    switch (handlerType()) {
    case NetworkManager:
        return QStringLiteral("NetworkManager");

    case ServerHandler:
        return QStringLiteral("UnknownServer");

    default:
        return QStringLiteral("UnknownHandler");
    };
}

/*!
 * \brief Sends a HEAD request.
 */
Response *RequestHandler::head(const Request &request)
{
    return send(HeadMethod, request, Body());
}

/*!
 * \brief Sends a GET request.
 */
Response *RequestHandler::get(const Request &request)
{
    return send(GetMethod, request, Body());
}

/*!
 * \brief Sends a POST request.
 */
Response *RequestHandler::post(const Request &request, const Body &body)
{
    return send(PostMethod, request, body);
}

/*!
 * \brief Sends a PUT request.
 */
Response *RequestHandler::put(const Request &request, const Body &body)
{
    return send(PutMethod, request, body);
}

/*!
 * \brief Sends a PATCH request.
 */
Response *RequestHandler::patch(const Request &request, const Body &body)
{
    return send(PatchMethod, request, body);
}

/*!
 * \brief Sends a DELETE request.
 */
Response *RequestHandler::deleteResource(const Request &request)
{
    return send(DeleteMethod, request, Body());
}

Response *RequestHandler::send(Method method, const Request &request, const Body &body)
{
    if (!isRequestSupported(request)) {
        restlinkWarning() << handlerName() << ": trying to send an unsupported request !";
        return nullptr;
    }

    Request finalRequest = request;
    for (RequestInterceptor *interceptor : std::as_const(d_ptr->interceptors))
        finalRequest = interceptor->intercept(method, finalRequest, body);
    return sendRequest(method, finalRequest, body);
}

/*!
 * \brief Returns the list of registered request interceptors.
 */
QList<RequestInterceptor *> RequestHandler::requestInterceptors() const
{
    return d_ptr->interceptors;
}

/*!
 * \brief Adds a new request interceptor.
 */
void RequestHandler::addRequestInterceptor(RequestInterceptor *interceptor)
{
    if (!d_ptr->interceptors.contains(interceptor))
        d_ptr->interceptors.append(interceptor);
}

/*!
 * \brief Removes a request interceptor.
 */
void RequestHandler::removeRequestInterceptor(RequestInterceptor *interceptor)
{
    d_ptr->interceptors.removeOne(interceptor);
}

/*!
 * \brief Checks if the request is supported by this handler.
 */
bool RequestHandler::isRequestSupported(const Request &request) const
{
    return supportedSchemes().contains(request.baseUrl().scheme());
}

/*!
 * \brief Initializes the response object with the associated request.
 */
void RequestHandler::initResponse(Response *response, const Request &request, Method method)
{
    Q_UNUSED(method);
    response->setRequest(request);
}

/*!
 * \brief Sends the request using the given method, final request, and body.
 *
 * This pure virtual method must be implemented by subclasses to handle the actual
 * transmission of the request and return the corresponding response.
 *
 * \param method The HTTP method to use.
 * \param request The request to send.
 * \param body The request body, if any.
 * \return A pointer to the response object.
 */

} // namespace RestLink
