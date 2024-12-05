#ifndef RESTLINK_APICACHE_H
#define RESTLINK_APICACHE_H

#include <RestLink/global.h>

#include <QtNetwork/qabstractnetworkcache.h>

namespace RestLink {

class ApiCachePrivate;
class RESTLINK_EXPORT ApiCache : public QAbstractNetworkCache
{
    Q_OBJECT
    Q_PROPERTY(qint64 maxCacheSize READ maxCacheSize WRITE setMaxCacheSize NOTIFY maxCacheSizeChanged FINAL)

public:
    explicit ApiCache(QObject *parent = nullptr);
    ~ApiCache();

    qint64 maxCacheSize() const;
    Q_SLOT void setMaxCacheSize(qint64 size);
    Q_SIGNAL void maxCacheSizeChanged(qint64 size);

    QNetworkCacheMetaData metaData(const QUrl &url) override;
    void updateMetaData(const QNetworkCacheMetaData &metaData) override;
    QIODevice *data(const QUrl &url) override;
    bool remove(const QUrl &url) override;
    qint64 cacheSize() const override;
    QIODevice *prepare(const QNetworkCacheMetaData &metaData) override;
    void insert(QIODevice *device) override;
    Q_SLOT void clear() override;

private:
    QScopedPointer<ApiCachePrivate> d;
};

}

#endif // RESTLINK_APICACHE_H
