#ifndef RESTLINKAPI_H
#define RESTLINKAPI_H

#include <RestLink/api.h>
#include <RestLink/apicache.h>

#include <QtQml/qqmlengine.h>

#include <QtNetwork/qnetworkaccessmanager.h>

#include <QtCore/qurl.h>

class RestLinkApi : public RestLink::Api
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QUrl configurationUrl READ apiConfigurationUrl WRITE setApiConfigurationUrl NOTIFY apiConfigurationUrlChanged)
    Q_PROPERTY(int cacheSize READ cacheSize WRITE setCacheSize NOTIFY cacheSizeChanged)
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged)

public:
    explicit RestLinkApi(QObject *parent = nullptr);
    ~RestLinkApi();

    Q_INVOKABLE void init();

    QUrl apiConfigurationUrl() const;
    Q_SLOT void setApiConfigurationUrl(const QUrl &url);
    Q_SIGNAL void apiConfigurationUrlChanged(const QUrl &url);

    int cacheSize() const;
    void setCacheSize(int size);
    Q_SIGNAL void cacheSizeChanged();

    bool isReady() const;
    void setReady(bool ready = true);
    Q_SIGNAL void readyChanged();

    Q_INVOKABLE QString parameterName(int index) const;
    Q_INVOKABLE QVariant parameterValue(int index) const;
    Q_INVOKABLE int parameterScope(int index) const;
    Q_INVOKABLE bool isParameterEnabled(int index) const;
    void registerParameters();

private:
    QUrl m_apiConfigUrl;
    bool m_ready;

    QNetworkAccessManager *m_net;
    RestLink::ApiCache *m_cache;
};

#endif // RESTLINKAPI_H
