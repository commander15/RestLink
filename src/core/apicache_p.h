#ifndef RESTLINK_APICACHE_P_H
#define RESTLINK_APICACHE_P_H

#include "apicache.h"

namespace RestLink {

class ApiCachePrivate
{
public:
    ApiCachePrivate(ApiCache *qq);

    void logCacheItem(const QNetworkCacheMetaData &meta, QIODevice *device);

    ApiCache *q;
};

}

#endif // RESTLINK_APICACHE_P_H
