#include "apicache.h"
#include "apicache_p.h"

#include <RestLink/debug.h>

#include <QtCore/qstandardpaths.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qfiledevice.h>

RestLink::ApiCache::ApiCache(QObject *parent) :
    QNetworkDiskCache(parent),
    d(new ApiCachePrivate(this))
{
    static int index = 0;
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    cacheDir.append("/RestLink/API" + QString::number(index++));

    setCacheDirectory(cacheDir);
    setMaximumCacheSize(5242880LL);
}

RestLink::ApiCache::~ApiCache()
{
}

QIODevice *RestLink::ApiCache::prepare(const QNetworkCacheMetaData &metaData)
{
    QIODevice *device = QNetworkDiskCache::prepare(metaData);
    d->logCacheItem(metaData, device);
    return device;
}

RestLink::ApiCachePrivate::ApiCachePrivate(ApiCache *qq) :
    q(qq)
{
}

void RestLink::ApiCachePrivate::logCacheItem(const QNetworkCacheMetaData &meta, QIODevice *device)
{
    return;
    restlinkInfo() << meta.url().toString() << " cached until " << meta.expirationDate()
                   << (device ? " on " + static_cast<QFileDevice *>(device)->fileName() : "");
}
