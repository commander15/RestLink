#include "networkmanager.h"

#include <RestLink/cache.h>

#include <QtNetwork/qnetworkcookiejar.h>

namespace RestLink {

NetworkManager::NetworkManager(QObject *parent)
    : QNetworkAccessManager{parent}
{
    setRedirectPolicy(QNetworkRequest::SameOriginRedirectPolicy);

    setCache(new Cache(this));
    setCookieJar(new QNetworkCookieJar(this));
}

}
