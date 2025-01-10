#ifndef RESTLINK_COOKIEJAR_H
#define RESTLINK_COOKIEJAR_H

#include <RestLink/global.h>

#include <QtNetwork/qnetworkcookiejar.h>

namespace RestLink {

class RESTLINK_EXPORT CookieJar : public QNetworkCookieJar
{
public:
    explicit CookieJar(QObject *parent = nullptr);
};

} // namespace RestLink

#endif // RESTLINK_COOKIEJAR_H
