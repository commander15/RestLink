#ifndef RESTLINK_CACHE_H
#define RESTLINK_CACHE_H

#include <RestLink/global.h>

#include <QtNetwork/qabstractnetworkcache.h>

namespace RestLink {

class CachePrivate;
class RESTLINK_EXPORT Cache : public QAbstractNetworkCache
{
    Q_OBJECT
    Q_PROPERTY(qint64 maxCacheSize READ maxCacheSize WRITE setMaxCacheSize NOTIFY maxCacheSizeChanged FINAL)

public:
    explicit Cache(QObject *parent = nullptr);
    ~Cache();

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
    QScopedPointer<CachePrivate> d;
};

}

#endif // RESTLINK_CACHE_H
