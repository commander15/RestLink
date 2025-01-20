#ifndef REQUEST_H
#define REQUEST_H

#include <RestLink/parameter.h>
#include <RestLink/parameterlist.h>
#include <RestLink/response.h>

#include <QtCore/qobject.h>

#include <QtQml/qqmlengine.h>

#include "api.h"

namespace RestLink {

class RequestInterface;
class Response;

namespace Qml {

class Request : public QObject
{
    QML_NAMED_ELEMENT(RequestBase)
    QML_ADDED_IN_VERSION(1, 0)

    Q_OBJECT
    Q_PROPERTY(int operation MEMBER m_operation NOTIFY operationChanged)
    Q_PROPERTY(QString endpoint MEMBER m_endpoint NOTIFY endpointChanged)
    Q_PROPERTY(QVariant body MEMBER m_body NOTIFY bodyChanged)
    Q_PROPERTY(bool running READ isRunning NOTIFY finished)
    Q_PROPERTY(bool finished READ isFinished NOTIFY finished)
    Q_PROPERTY(RestLink::Qml::Api* api MEMBER m_api NOTIFY apiChanged)

public:
    explicit Request(QObject *parent = nullptr);

    bool isRunning() const;
    bool isFinished() const;

    static void fillParameters(RequestInterface *interface, QObject *request);
    static ParameterList<> parameters(QObject *request);

public slots:
    void run();

signals:
    void operationChanged();
    void endpointChanged();
    void bodyChanged();
    void apiChanged();

    void finished(Response *response);

private:
    int m_operation;
    QString m_endpoint;
    QVariant m_body;

    Response *m_response;
    Api *m_api;
};

class RequestParameter : public QObject
{
    QML_NAMED_ELEMENT(RequestParameterBase)
    QML_ADDED_IN_VERSION(1, 0)

    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)
    Q_PROPERTY(QVariant value MEMBER m_value NOTIFY valueChanged)
    Q_PROPERTY(int type MEMBER m_type NOTIFY typeChanged)
    Q_PROPERTY(bool enabled MEMBER m_enabled NOTIFY enabledChanged)

public:
    explicit RequestParameter(QObject *parent = nullptr);

    Parameter parameter() const;
    bool isEnabled() const;

signals:
    void nameChanged();
    void valueChanged();
    void typeChanged();
    void enabledChanged();

private:
    QString m_name;
    QVariant m_value;
    int m_type;
    bool m_enabled;
};

} // namespace Qml
} // namespace RestLink

#endif // REQUEST_H
