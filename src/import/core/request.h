#ifndef REQUEST_H
#define REQUEST_H

#include "requestparameter.h"

#include <RestLink/requesthandler.h>
#include <RestLink/response.h>

#include <QtCore/qobject.h>

#include <QtQml/qqmlengine.h>

namespace RestLink {

class RequestInterface;
class Response;

namespace Qml {

class Request : public QObject, public QQmlParserStatus
{
    QML_ELEMENT
    QML_ADDED_IN_VERSION(1, 0)

    Q_OBJECT
    Q_PROPERTY(Method method MEMBER m_method NOTIFY methodChanged FINAL)
    Q_PROPERTY(QString endpoint MEMBER m_endpoint NOTIFY endpointChanged FINAL)
    Q_PROPERTY(QVariant body MEMBER m_body NOTIFY bodyChanged FINAL)
    Q_PROPERTY(bool running READ isRunning NOTIFY finished FINAL)
    Q_PROPERTY(bool finished READ isFinished NOTIFY finished FINAL)
    Q_PROPERTY(RestLink::Response *response READ response NOTIFY started FINAL)
    Q_PROPERTY(RestLink::Qml::Api *api MEMBER m_api NOTIFY apiChanged FINAL)
    Q_PROPERTY(QQmlListProperty<RestLink::Qml::RequestParameter> parameters MEMBER m_parametersProperty NOTIFY parametersChanged FINAL)

    Q_CLASSINFO("DefaultProperty", "parameters")

    Q_INTERFACES(QQmlParserStatus)

public:
    enum Method {
        Get = RequestHandler::GetMethod,
        Post = RequestHandler::PostMethod,
        Put = RequestHandler::PutMethod,
        Patch = RequestHandler::PatchMethod,
        Delete = RequestHandler::DeleteMethod
    };
    Q_ENUM(Method)

    explicit Request(QObject *parent = nullptr);

    bool isRunning() const;
    bool isFinished() const;
    Response *response() const;

    void classBegin() override;
    void componentComplete() override;

public slots:
    void run();

signals:
    void methodChanged();
    void endpointChanged();
    void bodyChanged();
    void apiChanged();

    void parametersChanged();

    void started();
    void finished();

private:
    Method m_method;
    QString m_endpoint;
    QVariant m_body;

    Response *m_response;
    Api *m_api;

    QList<RequestParameter *> m_parameters;
    QQmlListProperty<RequestParameter> m_parametersProperty;
};

} // namespace Qml
} // namespace RestLink

#endif // REQUEST_H
