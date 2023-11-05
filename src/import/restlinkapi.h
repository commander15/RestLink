#ifndef RESTLINKAPI_H
#define RESTLINKAPI_H

#include <RestLink/api.h>

#include <QtNetwork/qnetworkdiskcache.h>

#include <QtCore/qurl.h>

class RestLinkApi : public RestLink::Api
{
    Q_OBJECT
    Q_PROPERTY(QUrl apiConfigurationUrl READ apiConfigurationUrl WRITE setApiConfigurationUrl NOTIFY apiConfigurationUrlChanged)
    Q_PROPERTY(QString cacheDir READ cacheDir WRITE setCacheDir NOTIFY cacheDirChanged)
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged)

public:
    explicit RestLinkApi(QObject *parent = nullptr);
    ~RestLinkApi();

    QUrl apiConfigurationUrl() const;
    Q_SLOT void setApiConfigurationUrl(const QUrl &url);
    Q_SIGNAL void apiConfigurationUrlChanged(const QUrl &url);

    QString cacheDir() const;
    Q_SLOT void setCacheDir(const QString &dir);
    Q_SIGNAL void cacheDirChanged();

    bool isReady() const;
    void setReady(bool ready = true);
    Q_SIGNAL void readyChanged();

    Q_INVOKABLE QString parameterName(int index) const;
    Q_INVOKABLE QVariant parameterValue(int index) const;
    Q_INVOKABLE int parameterScope(int index) const;
    Q_INVOKABLE bool isParameterEnabled(int index) const;
    Q_INVOKABLE void registerParameters();

private:
    QUrl m_apiConfigUrl;
    bool m_ready;

    QNetworkDiskCache *m_cache;
};

#endif // RESTLINKAPI_H
