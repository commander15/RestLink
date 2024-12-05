#ifndef RESTLINK_API_H
#define RESTLINK_API_H

#include "qabstractnetworkcache.h"
#include <RestLink/global.h>
#include <RestLink/apibase.h>

#include <QtNetwork/qsslerror.h>

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>

class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;
class QSslError;

namespace RestLink {
class ApiReply;
class ApiConfigurationDownload;

class ApiPrivate;
class RESTLINK_EXPORT Api : public ApiBase
{
    Q_OBJECT
    Q_PROPERTY(QString apiName READ apiName WRITE setApiName NOTIFY apiNameChanged)
    Q_PROPERTY(int apiVersion READ apiVersion WRITE setApiVersion NOTIFY apiVersionChanged)
    Q_PROPERTY(QUrl apiUrl READ apiUrl WRITE setApiUrl NOTIFY apiUrlChanged)
    Q_PROPERTY(int apiParameterCount READ apiParameterCount NOTIFY apiParametersChanged)
    Q_PROPERTY(QString userAgent READ userAgent WRITE setUserAgent NOTIFY userAgentChanged)

public:
    explicit Api(QObject *parent = nullptr);
    ~Api();

    QString apiName() const;
    Q_SLOT void setApiName(const QString &name);
    Q_SIGNAL void apiNameChanged(const QString &name);

    int apiVersion() const;
    Q_SLOT void setApiVersion(int version);
    Q_SIGNAL void apiVersionChanged(int version);

    QUrl apiUrl() const override;
    Q_SLOT void setApiUrl(const QUrl &url);
    Q_SIGNAL void apiUrlChanged(const QUrl &url);

    ApiRequestParameter apiParameter(int index) const;
    int apiParameterCount() const;
    QList<ApiRequestParameter> apiParameters() const override;
    int addParameter(const ApiRequestParameter &parameter);
    void addParameters(const QList<ApiRequestParameter> &parameters);
    void setApiParameters(const QList<ApiRequestParameter> &parameters);
    Q_SIGNAL void apiParametersChanged();

    QString userAgent() const override;
    Q_SLOT void setUserAgent(const QString &agent);
    Q_SIGNAL void userAgentChanged(const QString &agent);

    void configureApi(const QUrl &url);
    bool configureApi(const QJsonObject &config);
    Q_SIGNAL void configurationCompleted();
    Q_SIGNAL void configurationFailed();

protected:
    ApiReply *createApiReply(const ApiRequest &request, QNetworkReply *netReply) override;
    QNetworkRequest createNetworkRequest(const ApiRequest &request) override;

    Q_SLOT virtual void processSslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

    friend class ApiPrivate;
    friend class ApiConfigurationDownload;
};

}

#endif // RESTLINK_API_H
