#ifndef RESTLINK_HTTPUTILS_H
#define RESTLINK_HTTPUTILS_H

#include <RestLink/global.h>

namespace RestLink {

class RESTLINK_EXPORT HttpUtils
{
public:
    static QString reasonPhrase(int code);

private:
    static const QMap<int, QByteArray> s_httpStatusCodes;
};

}

#endif // RESTLINK_HTTPUTILS_H
