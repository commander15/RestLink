#ifndef RESTLINK_API_H
#define RESTLINK_API_H

#include <RestLink/global.h>

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>

class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;
class QSslError;

namespace RestLink {
class ApiRequest;
class ApiRequestParameter;
class ApiReply;
class ApiConfigurationDownload;

class ApiPrivate;
class RESTLINK_EXPORT Api : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString apiName READ apiName WRITE setApiName NOTIFY apiNameChanged)
    Q_PROPERTY(int apiVersion READ apiVersion WRITE setApiVersion NOTIFY apiVersionChanged)
    Q_PROPERTY(QUrl apiUrl READ apiUrl WRITE setApiUrl NOTIFY apiUrlChanged)
    Q_PROPERTY(QString userAgent READ userAgent WRITE setUserAgent NOTIFY userAgentChanged)
    Q_PROPERTY(int apiParameterCount READ apiParameterCount NOTIFY apiParametersChanged)

public:
    explicit Api(QObject *parent = nullptr);
    Api(QNetworkAccessManager *netMan, QObject *parent = nullptr);
    ~Api();

    QString apiName() const;
    Q_SLOT void setApiName(const QString &name);
    Q_SIGNAL void apiNameChanged(const QString &name);

    int apiVersion() const;
    Q_SLOT void setApiVersion(int version);
    Q_SIGNAL void apiVersionChanged(int version);

    QUrl apiUrl() const;
    Q_SLOT void setApiUrl(const QUrl &url);
    Q_SIGNAL void apiUrlChanged(const QUrl &url);

    QString userAgent() const;
    Q_SLOT void setUserAgent(const QString &agent);
    Q_SIGNAL void userAgentChanged(const QString &agent);

    ApiReply *run(const ApiRequest &request);

    ApiRequestParameter apiParameter(int index) const;
    int apiParameterCount() const;
    QList<ApiRequestParameter> apiParameters() const;
    int addParameter(const ApiRequestParameter &parameter);
    void addParameters(const QList<ApiRequestParameter> &parameters);
    void setApiParameters(const QList<ApiRequestParameter> &parameters);
    Q_SIGNAL void apiParametersChanged();

    virtual bool canConfigureApi(const QJsonObject &config) const;
    void configureApi(const QUrl &url);
    virtual void configureApi(const QJsonObject &config);
    Q_SIGNAL void apiConfigured();
    Q_SIGNAL void apiConfigurationFailed(const QString &error);

    ApiConfigurationDownload *downloadApiConfiguration(const QUrl &url);

    QNetworkAccessManager *networkAccessManager() const;

protected:
    Q_SLOT virtual void processSslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

    virtual QNetworkRequest createNetworkRequest(const ApiRequest &request);
    QNetworkReply *createNetworkReply(const QByteArray &verb, const QNetworkRequest &request, const QByteArray &data, int dataType);

    QScopedPointer<ApiPrivate> d;

    friend class ApiPrivate;
    friend class ApiConfigurationDownload;
};

}

#endif // RESTLINK_API_H
