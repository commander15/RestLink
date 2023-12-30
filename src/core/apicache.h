#ifndef RESTLINK_APICACHE_H
#define RESTLINK_APICACHE_H

#include <RestLink/global.h>

#include <QtNetwork/qnetworkdiskcache.h>

namespace RestLink {

class ApiCachePrivate;
class RESTLINK_EXPORT ApiCache : public QNetworkDiskCache
{
    Q_OBJECT

public:
    explicit ApiCache(QObject *parent = nullptr);
    ~ApiCache();

    QIODevice *prepare(const QNetworkCacheMetaData &metaData) override;

private:
    QScopedPointer<ApiCachePrivate> d;
};

}

#endif // RESTLINK_APICACHE_H
