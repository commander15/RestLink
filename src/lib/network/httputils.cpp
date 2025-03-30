#include "httputils.h"
#include "httputils_p.h"

#include <QtCore/qmap.h>

namespace RestLink {

QString HttpUtils::verbString(ApiBase::Operation operation)
{
    switch (operation) {
    case ApiBase::HeadOperation:
        return QStringLiteral("HEAD");
    case ApiBase::GetOperation:
        return QStringLiteral("GET");
    case ApiBase::PostOperation:
        return QStringLiteral("POST");
    case ApiBase::PutOperation:
        return QStringLiteral("PUT");
    case ApiBase::PatchOperation:
        return QStringLiteral("PATCH");
    case ApiBase::DeleteOperation:
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
