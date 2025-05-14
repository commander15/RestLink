#include "requestinterceptor.h"

#include <RestLink/request.h>
#include <RestLink/httputils.h>
#include <qurl.h>

namespace RestLink {

/**
 * @class RequestInterceptor
 *
 * @brief Abstract class for intercepting and modifying HTTP requests before they are sent.
 *
 * The RequestInterceptor class allows for the modification of HTTP requests before they are executed.
 * By implementing the intercept method, custom logic can be added to modify request headers, URL, or any other part of the request.
 * This is particularly useful for adding authentication tokens, modifying headers, or logging requests.
 *
 * @see ApiBase, Request
 */

/**
 * @fn RestLink::RequestInterceptor::intercept(RequestHandler::Method method, const Request &request, const Body &body)
 * @brief Intercepts and modifies a request before it is sent.
 *
 * This method is called before the HTTP request is sent. It allows for modifications to the request such as altering the URL or adding headers.
 *
 * @param method The HTTP method being performed (e.g., GET, POST, PUT, ...).
 * @param request The request to be modified.
 * @param body The body of the request.
 *
 * @return The modified request object that will be sent.
 */

/**
 * @brief Constructs the interceptor with a given logging category and level.
 * @param category The logging category name (e.g., "restlink.request").
 * @param enableForLevel The minimum log level for which logging is enabled.
 */
LogRequestInterceptor::LogRequestInterceptor(const char *category, QtMsgType enableForLevel)
    : m_category(new QLoggingCategory(category, enableForLevel))
    , m_deleteCategory(true)
{
}

/**
 * @brief Constructs the interceptor using an existing QLoggingCategory pointer.
 * @param category Pointer to an existing QLoggingCategory used for logging.
 *        Ownership is not transferred; the interceptor will not delete it.
 */
LogRequestInterceptor::LogRequestInterceptor(QLoggingCategory *category)
    : m_category(category)
    , m_deleteCategory(false)
{
}

/**
 * @brief Destructor for LogRequestInterceptor.
 * Deletes the QLoggingCategory if it was created by this interceptor.
 */
LogRequestInterceptor::~LogRequestInterceptor()
{
    if (m_deleteCategory)
        delete m_category;
}

/**
 * @brief Intercepts an incoming request and logs it.
 *
 * the request is logged with format:
 * HTTP_VERB url (without secrets such as API key, never).
 * e.g. GET
 *
 * @param method The HTTP method of the request.
 * @param request The incoming request data.
 * @param body The body of the request.
 * @return The original request, unmodified.
 */
Request LogRequestInterceptor::intercept(RequestHandler::Method method, const Request &request, const Body &body)
{
    qCInfo(*m_category).noquote().nospace()
        << HttpUtils::verbString(method) << ' '
        << request.url(Request::PublicUrl).toString(QUrl::DecodeReserved);
    return request;
}

}
