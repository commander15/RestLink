#include "abstractrequestinterceptor.h"

#include <RestLink/request.h>
#include <RestLink/httputils.h>

#include <QtCore/qurl.h>

namespace RestLink {

/**
 * @class AbstractRequestInterceptor
 *
 * @brief Interface for intercepting and modifying HTTP requests and their bodies before dispatch.
 *
 * Implement this class to apply custom logic to HTTP requests before they are sent.
 * Typical use cases include injecting authentication tokens, customizing headers,
 * modifying the request URL, or altering the request body.
 *
 * This is useful for logging, request signing, transforming payloads, or enforcing global rules.
 * The interceptor is called just before the request is dispatched.
 *
 * @see ApiBase, Request, Body, AbstractRequestHandler::Method
 */

/**
 * @fn AbstractRequestInterceptor::intercept(RequestHandler::Method method, Request &request, Body &body)
 *
 * @brief Called before a request is sent, allowing modification of both request and body.
 *
 * Use this method to alter the HTTP method, headers, URL, or the body content prior to transmission.
 *
 * @param method The HTTP method (GET, POST, PUT, etc.).
 * @param request The request object to be modified.
 * @param body The body associated with the request, which can also be modified.
 */

/**
 * @class LogRequestInterceptor
 *
 * @brief Helper class to create a simple logger for RestLink requests.
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
 * @brief Logs the HTTP method and request URL.
 *
 * Sensitive data such as api tokens will never be shown.
 *
 * @param method HTTP method used.
 * @param request Request to log.
 * @param body Request body.
 */
void LogRequestInterceptor::intercept(AbstractRequestHandler::Method method, Request &request, Body &body)
{
    qCInfo(*m_category).noquote().nospace()
        << HttpUtils::verbString(method) << ' '
        << request.url(Request::PublicUrl).toString(QUrl::DecodeReserved);
}

}
