#include "apicache.h"
#include "apicache_p.h"

#include <RestLink/debug.h>

#include <QtCore/qstandardpaths.h>
#include <QtCore/qurl.h>
#include <QtCore/qurlquery.h>

RestLink::ApiCache::ApiCache(QObject *parent) :
    QAbstractNetworkCache(parent),
    d(new ApiCachePrivate(this))
{
}

RestLink::ApiCache::~ApiCache()
{
}

qint64 RestLink::ApiCache::maxCacheSize() const
{
    return d->maximumCacheSize();
}

void RestLink::ApiCache::setMaxCacheSize(qint64 size)
{
    if (d->maximumCacheSize() != size) {
        d->setMaximumCacheSize(size);
        emit maxCacheSizeChanged(size);
    }
}

QNetworkCacheMetaData RestLink::ApiCache::metaData(const QUrl &url)
{
    return d->metaData(d->cacheUrl(url));
}

void RestLink::ApiCache::updateMetaData(const QNetworkCacheMetaData &metaData)
{
    d->updateMetaData(d->cacheMetaData(metaData));
}

QIODevice *RestLink::ApiCache::data(const QUrl &url)
{
    return d->data(d->cacheUrl(url));
}

bool RestLink::ApiCache::remove(const QUrl &url)
{
    return d->remove(d->cacheUrl(url));
}

qint64 RestLink::ApiCache::cacheSize() const
{
    return d->cacheSize();
}

QIODevice *RestLink::ApiCache::prepare(const QNetworkCacheMetaData &metaData)
{
    return d->prepare(d->cacheMetaData(metaData));
}

void RestLink::ApiCache::insert(QIODevice *device)
{
    d->insert(device);
}

void RestLink::ApiCache::clear()
{
    d->clear();
}

RestLink::ApiCachePrivate::ApiCachePrivate(ApiCache *qq) :
    q(qq)
{
    static int index = 0;
    setCacheDirectory(generateCacheDir(QString::number(index++)));

    setMaximumCacheSize(5242880LL);
}

QNetworkCacheMetaData RestLink::ApiCachePrivate::cacheMetaData(QNetworkCacheMetaData metaData) const
{
    metaData.setUrl(cacheUrl(metaData.url()));
    // ToDo: Handle in header secrets
    return metaData;
}

QUrl RestLink::ApiCachePrivate::cacheUrl(QUrl url) const
{
    QUrlQuery query(url.query());
    // ToDo: Handle in url secrets
    url.setQuery(query);
    return url;
}

QString RestLink::ApiCachePrivate::generateCacheDir(const QString &name)
{
    static const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/RestLink/API/";
    return cacheDir + name;
}
