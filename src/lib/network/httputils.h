#ifndef RESTLINK_HTTPUTILS_H
#define RESTLINK_HTTPUTILS_H

#include <RestLink/global.h>
#include <RestLink/api.h>

namespace RestLink {

class RESTLINK_EXPORT HttpUtils
{
public:
    static QString methodString(Api::Operation operation);
    static QString reasonPhrase(int code);

private:
    static const QMap<int, QByteArray> s_httpStatusCodes;
};

}

#endif // RESTLINK_HTTPUTILS_H
