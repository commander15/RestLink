#include "requestinterceptor.h"

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
 * @fn RestLink::RequestInterceptor::intercept(const Request &request, const Body &body, RequestHandler::Method method)
 * @brief Intercepts and modifies a request before it is sent.
 *
 * This method is called before the HTTP request is sent. It allows for modifications to the request such as altering the URL, adding headers, or changing the request body.
 *
 * @param request The request to be modified.
 * @param body The body of the request.
 * @param method The HTTP method being performed (e.g., GET, POST, PUT).
 *
 * @return The modified request object that will be sent.
 */
}
