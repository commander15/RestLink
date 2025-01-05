#include "cache.h"
#include "cache_p.h"

#include <RestLink/debug.h>

#include <QtCore/qstandardpaths.h>
#include <QtCore/qurl.h>
#include <QtCore/qurlquery.h>

RestLink::Cache::Cache(QObject *parent) :
    QAbstractNetworkCache(parent),
    d(new CachePrivate(this))
{
}

RestLink::Cache::~Cache()
{
}

qint64 RestLink::Cache::maxCacheSize() const
{
    return d->maximumCacheSize();
}

void RestLink::Cache::setMaxCacheSize(qint64 size)
{
    if (d->maximumCacheSize() != size) {
        d->setMaximumCacheSize(size);
        emit maxCacheSizeChanged(size);
    }
}

QNetworkCacheMetaData RestLink::Cache::metaData(const QUrl &url)
{
    return d->metaData(d->cacheUrl(url));
}

void RestLink::Cache::updateMetaData(const QNetworkCacheMetaData &metaData)
{
    d->updateMetaData(d->cacheMetaData(metaData));
}

QIODevice *RestLink::Cache::data(const QUrl &url)
{
    return d->data(d->cacheUrl(url));
}

bool RestLink::Cache::remove(const QUrl &url)
{
    return d->remove(d->cacheUrl(url));
}

qint64 RestLink::Cache::cacheSize() const
{
    return d->cacheSize();
}

QIODevice *RestLink::Cache::prepare(const QNetworkCacheMetaData &metaData)
{
    return d->prepare(d->cacheMetaData(metaData));
}

void RestLink::Cache::insert(QIODevice *device)
{
    d->insert(device);
}

void RestLink::Cache::clear()
{
    d->clear();
}

RestLink::CachePrivate::CachePrivate(Cache *qq) :
    q(qq)
{
    static int index = 0;
    setCacheDirectory(generateCacheDir(QString::number(index++)));

    setMaximumCacheSize(5242880LL);
}

QNetworkCacheMetaData RestLink::CachePrivate::cacheMetaData(QNetworkCacheMetaData metaData) const
{
    metaData.setUrl(cacheUrl(metaData.url()));
    // ToDo: Handle in header secrets
    return metaData;
}

QUrl RestLink::CachePrivate::cacheUrl(QUrl url) const
{
    QUrlQuery query(url.query());
    // ToDo: Handle in url secrets
    url.setQuery(query);
    return url;
}

QString RestLink::CachePrivate::generateCacheDir(const QString &name)
{
    static const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/RestLink/API/";
    return cacheDir + name;
}

QIODevice *RestLink::CachePrivate::prepare(const QNetworkCacheMetaData &metaData)
{
    const QNetworkCacheMetaData::RawHeaderList headers = metaData.rawHeaders();

    // Check for Cache-Control: private directive
    for (const QPair<QByteArray, QByteArray> &header : headers) {
        if (header.first.toLower() == "cache-control") {
            const QByteArray &value = header.second.toLower();
            if (value.contains("private")) {
                // Avoid caching the request if Cache-Control is private
                return nullptr;
            }
        }
    }

    // Default behavior if no private directive is found
    return QNetworkDiskCache::prepare(metaData);
}

