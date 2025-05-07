#ifndef TYPES_H
#define TYPES_H

#include <QtQml/qqmlengine.h>

#include <RestLink/api.h>
#include <RestLink/response.h>
#include <RestLink/cache.h>
#include <RestLink/cookiejar.h>
#include <RestLink/networkmanager.h>

struct ForeignApi
{
    Q_GADGET
    QML_NAMED_ELEMENT(BasicApi)
    QML_FOREIGN(RestLink::Api)
    QML_ADDED_IN_VERSION(1, 0)
};

struct ForeignResponse
{
    Q_GADGET
    QML_ANONYMOUS
    QML_FOREIGN(RestLink::Response)
    QML_ADDED_IN_VERSION(1, 0)
};

struct ForeignCache
{
    Q_GADGET
    QML_NAMED_ELEMENT(Cache)
    QML_FOREIGN(RestLink::Cache)
    QML_ADDED_IN_VERSION(2, 0)
};

struct ForeignCookieJar
{
    Q_GADGET
    QML_NAMED_ELEMENT(CookieJar)
    QML_FOREIGN(RestLink::CookieJar)
    QML_ADDED_IN_VERSION(2, 0)
};

struct ForeignNetworkManager
{
    Q_GADGET
    QML_NAMED_ELEMENT(NetworkManager)
    QML_FOREIGN(RestLink::NetworkManager)
    QML_ADDED_IN_VERSION(2, 0)
};

#endif // TYPES_H
