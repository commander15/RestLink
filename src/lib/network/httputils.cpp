#include "httputils.h"
#include "httputils_p.h"

namespace RestLink {

/*!
 * \class HttpUtils
 * \brief Utility class providing helper functions for HTTP-related operations.
 *
 * This class offers static methods to convert HTTP methods to strings and
 * retrieve reason phrases for HTTP status codes.
 */

/*!
 * \brief Converts an HTTP method enum to its string representation.
 *
 * \param method The HTTP method as defined in RequestHandler::Method.
 * \return The corresponding HTTP verb as a QString (e.g., "GET", "POST").
 */
QString HttpUtils::verbString(AbstractRequestHandler::Method method)
{
    switch (method) {
    case AbstractRequestHandler::HeadMethod:
        return QStringLiteral("HEAD");
    case AbstractRequestHandler::GetMethod:
        return QStringLiteral("GET");
    case AbstractRequestHandler::PostMethod:
        return QStringLiteral("POST");
    case AbstractRequestHandler::PutMethod:
        return QStringLiteral("PUT");
    case AbstractRequestHandler::PatchMethod:
        return QStringLiteral("PATCH");
    case AbstractRequestHandler::DeleteMethod:
        return QStringLiteral("DELETE");
    default:
        return QString();
    }
}

/*!
 * \brief Returns the standard reason phrase for a given HTTP status code.
 *
 * \param code The HTTP status code (e.g., 200, 404).
 * \return The reason phrase associated with the status code.
 */
QString HttpUtils::reasonPhrase(int code)
{
    return HttpUtilsPrivate::httpStatusCodes.value(code);
}

}
