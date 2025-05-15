#include "abstractrequesthandler.h"
#include "abstractrequesthandler_p.h"

#include <RestLink/debug.h>
#include <RestLink/request.h>
#include <RestLink/body.h>
#include <RestLink/response.h>
#include <RestLink/abstractrequestinterceptor.h>
#include <RestLink/httputils.h>

namespace RestLink {

/*!
 * \class RestLink::AbstractRequestHandler
 * \brief The AbstractRequestHandler class defines an abstract base for sending HTTP-like requests.
 *
 * It provides generic methods to send requests using standard HTTP methods (GET, POST, etc.).
 * Subclasses must implement scheme support and request sending behavior.
 *
 * \see NetworkManager
 */

/*!
 * \enum AbstractRequestHandler::Method
 * \brief Defines supported HTTP methods for handling outgoing requests.
 *
 * This enum is used to identify the HTTP method associated with a request.
 *
 * \var AbstractRequestHandler::Method AbstractRequestHandler::HeadMethod
 * Represents the HTTP HEAD method, used to retrieve headers without the response body.
 * \var AbstractRequestHandler::Method AbstractRequestHandler::GetMethod
 * Represents the HTTP GET method, used to retrieve data from the server.
 * \var AbstractRequestHandler::Method AbstractRequestHandler::PostMethod
 * Represents the HTTP POST method, used to submit data to the server.
 * \var AbstractRequestHandler::Method AbstractRequestHandler::PutMethod
 * Represents the HTTP PUT method, used to update or replace existing resources.
 * \var AbstractRequestHandler::Method AbstractRequestHandler::PatchMethod
 * Represents the HTTP PATCH method, used to apply partial modifications to a resource.
 * \var AbstractRequestHandler::Method AbstractRequestHandler::DeleteMethod
 * Represents the HTTP DELETE method, used to remove a resource.
 * \var AbstractRequestHandler::Method AbstractRequestHandler::UnknownMethod
 * Represents an unknown or unsupported HTTP method.
 */

/*!
 * \enum AbstractRequestHandler::HandlerType
 * \brief Describes the type of request handler being used.
 *
 * This enum helps identify the underlying implementation responsible for processing the request.

 * \var AbstractRequestHandler::HandlerType AbstractRequestHandler::NetworkManager
 * Indicates that the handler is based on the NetworkManager class (typically used for client-side requests).
 * \var AbstractRequestHandler::HandlerType AbstractRequestHandler::ServerHandler
 * Indicates that the handler is based on the Server class (typically used for server-side processing).
 * \var AbstractRequestHandler::HandlerType AbstractRequestHandler::UnknownHandler
 * Indicates an unknown or custom handler type not explicitly defined by the framework.
 */


AbstractRequestHandler::AbstractRequestHandler()
    : d_ptr(new AbstractRequestHandlerPrivate())
{
}

/*!
 * \brief Destructor for RequestHandler.
 */
AbstractRequestHandler::~AbstractRequestHandler()
{
}

/*!
 * \brief Returns the name of the handler based on its type.
 * \note this method can be overriden on subclasses.
 */
QString AbstractRequestHandler::handlerName() const
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
Response *AbstractRequestHandler::head(const Request &request)
{
    return send(HeadMethod, request, Body());
}

/*!
 * \brief Sends a GET request.
 */
Response *AbstractRequestHandler::get(const Request &request)
{
    return send(GetMethod, request, Body());
}

/*!
 * \brief Sends a POST request.
 */
Response *AbstractRequestHandler::post(const Request &request, const Body &body)
{
    return send(PostMethod, request, body);
}

/*!
 * \brief Sends a PUT request.
 */
Response *AbstractRequestHandler::put(const Request &request, const Body &body)
{
    return send(PutMethod, request, body);
}

/*!
 * \brief Sends a PATCH request.
 */
Response *AbstractRequestHandler::patch(const Request &request, const Body &body)
{
    return send(PatchMethod, request, body);
}

/*!
 * \brief Sends a DELETE request.
 */
Response *AbstractRequestHandler::deleteResource(const Request &request)
{
    return send(DeleteMethod, request, Body());
}

Response *AbstractRequestHandler::send(Method method, const Request &request, const Body &body)
{
    if (!isRequestSupported(request)) {
        restlinkWarning() << handlerName() << ": trying to send an unsupported request !";
        return nullptr;
    }

    Request finalRequest = request;
    Body finalBody = body;
    for (AbstractRequestInterceptor *interceptor : std::as_const(d_ptr->interceptors))
        interceptor->intercept(method, finalRequest, finalBody);
    return sendRequest(method, finalRequest, finalBody);
}

/*!
 * \brief Returns the list of registered request interceptors.
 */
QList<AbstractRequestInterceptor *> AbstractRequestHandler::requestInterceptors() const
{
    return d_ptr->interceptors;
}

/*!
 * \brief Adds a new request interceptor.
 */
void AbstractRequestHandler::addRequestInterceptor(AbstractRequestInterceptor *interceptor)
{
    if (!d_ptr->interceptors.contains(interceptor))
        d_ptr->interceptors.append(interceptor);
}

/*!
 * \brief Removes a request interceptor.
 */
void AbstractRequestHandler::removeRequestInterceptor(AbstractRequestInterceptor *interceptor)
{
    d_ptr->interceptors.removeOne(interceptor);
}

/*!
 * \brief Checks if the request is supported by this handler.
 */
bool AbstractRequestHandler::isRequestSupported(const Request &request) const
{
    return supportedSchemes().contains(request.baseUrl().scheme());
}

/*!
 * \brief Initializes the response object with the associated request.
 *
 * It's recommended to use this method to initialize your response objects.
 */
void AbstractRequestHandler::initResponse(Response *response, const Request &request, Method method)
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
