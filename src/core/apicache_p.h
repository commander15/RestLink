#ifndef RESTLINK_APICACHE_P_H
#define RESTLINK_APICACHE_P_H

#include "apicache.h"

#include <QtNetwork/qnetworkdiskcache.h>

namespace RestLink {

class ApiCachePrivate : public QNetworkDiskCache
{
    Q_OBJECT

public:
    ApiCachePrivate(ApiCache *qq);

    QNetworkCacheMetaData cacheMetaData(QNetworkCacheMetaData metaData) const;
    QUrl cacheUrl(QUrl url) const;

    static QString generateCacheDir(const QString &name);

    ApiCache *q;


};

}

#endif // RESTLINK_APICACHE_P_H
