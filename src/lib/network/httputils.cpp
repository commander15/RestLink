#include "httputils.h"
#include "httputils_p.h"

namespace RestLink {

QString HttpUtils::verbString(RequestHandler::Method method)
{
    switch (method) {
    case RequestHandler::HeadMethod:
        return QStringLiteral("HEAD");
    case RequestHandler::GetMethod:
        return QStringLiteral("GET");
    case RequestHandler::PostMethod:
        return QStringLiteral("POST");
    case RequestHandler::PutMethod:
        return QStringLiteral("PUT");
    case RequestHandler::PatchMethod:
        return QStringLiteral("PATCH");
    case RequestHandler::DeleteMethod:
        return QStringLiteral("DELETE");
    default:
        return QString();
    }
}

QString HttpUtils::reasonPhrase(int code)
{
    return HttpUtilsPrivate::httpStatusCodes.value(code);
}

}
