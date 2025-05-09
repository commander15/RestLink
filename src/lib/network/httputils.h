#ifndef RESTLINK_HTTPUTILS_H
#define RESTLINK_HTTPUTILS_H

#include <RestLink/global.h>
#include <RestLink/requesthandler.h>

namespace RestLink {

class RESTLINK_EXPORT HttpUtils
{
public:
    static QString verbString(RequestHandler::Method method);
    static QString reasonPhrase(int code);
};

}

#endif // RESTLINK_HTTPUTILS_H
