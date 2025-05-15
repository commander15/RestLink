#ifndef RESTLINK_CACHE_P_H
#define RESTLINK_CACHE_P_H

#include "cache.h"

#include <QtNetwork/qnetworkdiskcache.h>

namespace RestLink {

class CachePrivate : public QNetworkDiskCache
{
    Q_OBJECT

public:
    CachePrivate(Cache *qq);

    QNetworkCacheMetaData cacheMetaData(QNetworkCacheMetaData metaData) const;
    QUrl cacheUrl(QUrl url) const;

    QIODevice *prepare(const QNetworkCacheMetaData &metaData) override;

    Cache *q;
};

}

#endif // RESTLINK_CACHE_P_H
