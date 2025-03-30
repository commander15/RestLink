#include "cache.h"
#include "cache_p.h"

#include <RestLink/debug.h>

#include <QtCore/qstandardpaths.h>
#include <QtCore/qurl.h>
#include <QtCore/qurlquery.h>

namespace RestLink {

/**
 * @class Cache
 * @brief A custom network cache implementation for RestLink.
 *
 * This class extends QAbstractNetworkCache to provide custom caching
 * functionality for the RestLink API. It interacts with CachePrivate to
 * manage cache storage and retrieval.
 */

Cache::Cache(QObject *parent) :
    QAbstractNetworkCache(parent),
    d(new CachePrivate(this))
{
}

/**
 * @brief Destructor for the Cache class.
 *
 * This destructor cleans up the cache private data.
 */
Cache::~Cache()
{
}

/**
 * @brief Returns the maximum allowed cache size.
 *
 * @return The maximum cache size in bytes.
 */
qint64 Cache::maxCacheSize() const
{
    return d->maximumCacheSize();
}

/**
 * @brief Sets the maximum cache size.
 *
 * If the new size is different from the current maximum, this function
 * updates the maximum cache size and emits the `maxCacheSizeChanged` signal.
 *
 * @param size The new maximum cache size in bytes.
 */
void Cache::setMaxCacheSize(qint64 size)
{
    if (d->maximumCacheSize() != size) {
        d->setMaximumCacheSize(size);
        emit maxCacheSizeChanged(size);
    }
}

/**
 * @brief Retrieves the metadata for a given URL.
 *
 * This function fetches the metadata associated with a URL in the cache.
 *
 * @param url The URL whose metadata needs to be retrieved.
 *
 * @return The metadata associated with the URL.
 */
QNetworkCacheMetaData Cache::metaData(const QUrl &url)
{
    return d->metaData(d->cacheUrl(url));
}

/**
 * @brief Updates the cache metadata with the provided data.
 *
 * @param metaData The new metadata to be used in the cache.
 */
void Cache::updateMetaData(const QNetworkCacheMetaData &metaData)
{
    d->updateMetaData(d->cacheMetaData(metaData));
}

/**
 * @brief Retrieves the cached data for a given URL.
 *
 * This function returns the cached data as a QIODevice object for a given
 * URL.
 *
 * @param url The URL whose cached data needs to be retrieved.
 *
 * @return A QIODevice object with the cached data, or nullptr if no data is found.
 */
QIODevice *Cache::data(const QUrl &url)
{
    return d->data(d->cacheUrl(url));
}

/**
 * @brief Removes the cached data for a given URL.
 *
 * @param url The URL whose cached data needs to be removed.
 *
 * @return True if the data was removed successfully, false otherwise.
 */
bool Cache::remove(const QUrl &url)
{
    return d->remove(d->cacheUrl(url));
}

/**
 * @brief Returns the total size of the cache.
 *
 * @return The current size of the cache in bytes.
 */
qint64 Cache::cacheSize() const
{
    return d->cacheSize();
}

/**
 * @brief Prepares a QIODevice to store data in the cache.
 *
 * This function prepares a QIODevice for writing data associated with a
 * cache metadata entry.
 *
 * @param metaData The metadata of the cache entry to prepare for.
 *
 * @return A QIODevice object for storing the cache data.
 */
QIODevice *Cache::prepare(const QNetworkCacheMetaData &metaData)
{
    return d->prepare(d->cacheMetaData(metaData));
}

/**
 * @brief Inserts data into the cache.
 *
 * This function inserts a QIODevice containing cache data into the cache.
 *
 * @param device The device containing the cache data.
 */
void Cache::insert(QIODevice *device)
{
    d->insert(device);
}

/**
 * @brief Clears the cache.
 *
 * This function clears all data from the cache.
 */
void Cache::clear()
{
    d->clear();
}

CachePrivate::CachePrivate(Cache *qq) :
    q(qq)
{
    static int index = 0;
    setCacheDirectory(generateCacheDir(QString::number(index++)));

    setMaximumCacheSize(5242880LL);  // Set the default cache size to 5MB
}

QNetworkCacheMetaData CachePrivate::cacheMetaData(QNetworkCacheMetaData metaData) const
{
    metaData.setUrl(cacheUrl(metaData.url()));
    // ToDo: Handle in header secrets
    return metaData;
}

QUrl CachePrivate::cacheUrl(QUrl url) const
{
    QUrlQuery query(url.query());
    // ToDo: Handle in url secrets
    url.setQuery(query);
    return url;
}

QString CachePrivate::generateCacheDir(const QString &name)
{
    static const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/RestLink/API/";
    return cacheDir + name;
}

QIODevice *CachePrivate::prepare(const QNetworkCacheMetaData &metaData)
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

    // Default behavior if no private directive found
    return QNetworkDiskCache::prepare(metaData);
}

} // namespace RestLink
