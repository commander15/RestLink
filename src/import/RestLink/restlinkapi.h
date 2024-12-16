#ifndef RESTLINKAPI_H
#define RESTLINKAPI_H

#include <RestLink/api.h>
#include <RestLink/apicache.h>

#include <QtQml/qqmlengine.h>

#include <QtNetwork/qnetworkaccessmanager.h>

#include <QtCore/qurl.h>

class RestLinkApi : public RestLink::Api
{
    QML_ELEMENT
    QML_ADDED_IN_VERSION(1, 0)

    Q_OBJECT
    Q_PROPERTY(bool ready READ isReady NOTIFY ready)
    Q_PROPERTY(QUrl configurationUrl READ apiConfigurationUrl WRITE setApiConfigurationUrl NOTIFY apiConfigurationUrlChanged)
    Q_PROPERTY(RestLink::ApiCache* cache READ cache WRITE setCache NOTIFY cacheChanged FINAL)

public:
    explicit RestLinkApi(QObject *parent = nullptr);
    ~RestLinkApi();

    Q_INVOKABLE void init();

    bool isReady() const;
    Q_SIGNAL void ready();

    QUrl apiConfigurationUrl() const;
    Q_SLOT void setApiConfigurationUrl(const QUrl &url);
    Q_SIGNAL void apiConfigurationUrlChanged(const QUrl &url);

    RestLink::ApiCache *cache() const;
    void setCache(RestLink::ApiCache *cache);
    Q_SIGNAL void cacheChanged();

    Q_INVOKABLE QString parameterName(int index) const;
    Q_INVOKABLE QVariant parameterValue(int index) const;
    Q_INVOKABLE int parameterScope(int index) const;
    Q_INVOKABLE bool isParameterEnabled(int index) const;
    void registerParameters();

private:
    QUrl m_apiConfigUrl;

    RestLink::ApiCache *m_cache;
};

#endif // RESTLINKAPI_H
