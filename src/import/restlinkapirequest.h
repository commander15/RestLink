#ifndef RESTLINKAPIREQUEST_H
#define RESTLINKAPIREQUEST_H

#include <RestLink/apirequest.h>
#include <RestLink/apireply.h>

#include <QtCore/qobject.h>

#include "restlinkapi.h"

class RestLinkApiRequest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString endpoint READ endpoint WRITE setEndpoint NOTIFY endpointChanged)
    Q_PROPERTY(QString data READ data WRITE setData NOTIFY dataChanged)
    Q_PROPERTY(int type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString response READ response NOTIFY finished)
    Q_PROPERTY(bool running READ isRunning NOTIFY finished)
    Q_PROPERTY(RestLink::ApiReply* apiReply READ apiReply NOTIFY apiReplyChanged)
    Q_PROPERTY(RestLinkApi* api READ api WRITE setApi)

public:
    explicit RestLinkApiRequest(QObject *parent = nullptr);
    ~RestLinkApiRequest();

    QString endpoint() const;
    Q_SLOT void setEndpoint(const QString &newEndpoint);
    Q_SIGNAL void endpointChanged();

    QString data() const;
    Q_SLOT void setData(const QString &data);
    Q_SIGNAL void dataChanged(const QString &data);

    int type() const;
    Q_SLOT void setType(int type);
    Q_SIGNAL void typeChanged(int type);

    QString response() const;

    bool isRunning() const;
    Q_SIGNAL void finished();

    Q_SLOT void run();

    RestLink::ApiReply *apiReply() const;
    Q_SIGNAL void apiReplyChanged();

    RestLinkApi *api() const;
    void setApi(RestLinkApi *api);

    static QList<RestLink::ApiRequestParameter> apiRequestParameters(const QObject *request);

private:
    QString m_endpoint;
    QString m_data;
    int m_type;

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

    RestLink::ApiRequestParameter requestParameter() const;

private:
    RestLink::ApiRequestParameter m_param;
};

#endif // RESTLINKAPIREQUEST_H
