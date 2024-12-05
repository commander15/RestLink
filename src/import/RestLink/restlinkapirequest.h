#ifndef RESTLINKAPIREQUEST_H
#define RESTLINKAPIREQUEST_H

#include <RestLink/apirequest.h>
#include <RestLink/apireply.h>

#include <QtQml/qqmlengine.h>

#include <QtCore/qobject.h>

#include "restlinkapi.h"

class RestLinkApiRequest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString endpoint READ endpoint WRITE setEndpoint NOTIFY endpointChanged)
    Q_PROPERTY(QVariant body READ body WRITE setBody NOTIFY bodyChanged)
    Q_PROPERTY(int operation READ operation WRITE setOperation NOTIFY operationChanged)
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
    Q_PROPERTY(RestLink::ApiReply* response READ response NOTIFY responseChanged)
    Q_PROPERTY(RestLinkApi* api READ api WRITE setApi NOTIFY apiChanged)
    QML_ELEMENT

public:
    explicit RestLinkApiRequest(QObject *parent = nullptr);
    ~RestLinkApiRequest();

    QString endpoint() const;
    void setEndpoint(const QString &newEndpoint);
    Q_SIGNAL void endpointChanged();

    QVariant body() const;
    void setBody(const QVariant &data);
    Q_SIGNAL void bodyChanged();

    int operation() const;
    void setOperation(int operation);
    Q_SIGNAL void operationChanged();

    bool isRunning() const;
    Q_SLOT void run();
    Q_SIGNAL void runningChanged();

    RestLink::ApiReply *response() const;
    Q_SIGNAL void responseChanged();

    RestLinkApi *api() const;
    void setApi(RestLinkApi *api);
    Q_SIGNAL void apiChanged();

    static QList<RestLink::ApiRequestParameter> apiRequestParameters(const QObject *request);

private:
    QString m_endpoint;
    QVariant m_body;
    int m_operation;

    RestLink::ApiReply *m_reply;
    RestLinkApi *m_api;
};

class RestLinkApiRequestParameter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int scope READ scope WRITE setScope NOTIFY scopeChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    QML_ELEMENT

public:
    RestLinkApiRequestParameter(QObject *parent = nullptr);
    ~RestLinkApiRequestParameter();

    QString name() const;
    Q_SLOT void setName(const QString &name);
    Q_SIGNAL void nameChanged();

    QVariant value() const;
    Q_SLOT void setValue(const QVariant &value);
    Q_SIGNAL void valueChanged();

    int scope() const;
    Q_SLOT void setScope(int scope);
    Q_SIGNAL void scopeChanged();

    bool isEnabled() const;
    Q_SLOT void setEnabled(bool enabled);
    Q_SIGNAL void enabledChanged();

    Q_SIGNAL void parameterChanged();

    RestLink::ApiRequestParameter requestParameter() const;

private:
    RestLink::ApiRequestParameter m_param;
};

#endif // RESTLINKAPIREQUEST_H
