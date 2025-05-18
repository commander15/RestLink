#ifndef REQUEST_H
#define REQUEST_H

#include "requestparameter.h"

#include <RestLink/abstractrequesthandler.h>
#include <RestLink/response.h>

#include <QtCore/qobject.h>

#include <QtQml/qqmlengine.h>

namespace RestLink {

class RequestInterface;
class Response;

namespace Qml {

class Body;

class Request : public QObject, public QQmlParserStatus
{
    QML_ELEMENT
    QML_ADDED_IN_VERSION(1, 0)

    Q_OBJECT
    Q_PROPERTY(Method method MEMBER m_method NOTIFY methodChanged FINAL)
    Q_PROPERTY(QString endpoint MEMBER m_endpoint NOTIFY endpointChanged FINAL)
    Q_PROPERTY(RestLink::Qml::Body* body READ body NOTIFY bodyChanged FINAL)
    Q_PROPERTY(bool autoRun MEMBER m_autoRun NOTIFY autoRunChanged)
    Q_PROPERTY(bool running READ isRunning NOTIFY finished FINAL)
    Q_PROPERTY(bool finished READ isFinished NOTIFY finished FINAL)
    Q_PROPERTY(RestLink::Response *response READ response NOTIFY started FINAL)
    Q_PROPERTY(RestLink::Qml::Api *api MEMBER m_api NOTIFY apiChanged FINAL)
    Q_PROPERTY(QQmlListProperty<RestLink::Qml::RequestParameter> parameters MEMBER m_parametersProperty NOTIFY parametersChanged FINAL)

    Q_CLASSINFO("DefaultProperty", "parameters")

    Q_INTERFACES(QQmlParserStatus)

public:
    enum Method {
        Get = AbstractRequestHandler::GetMethod,
        Post = AbstractRequestHandler::PostMethod,
        Put = AbstractRequestHandler::PutMethod,
        Patch = AbstractRequestHandler::PatchMethod,
        Delete = AbstractRequestHandler::DeleteMethod
    };
    Q_ENUM(Method)

    explicit Request(QObject *parent = nullptr);

    Body *body() const;

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
    void autoRunChanged();

    void started();
    void finished();

private:
    Method m_method;
    QString m_endpoint;
    Body *m_body;
    bool m_autoRun;

    Response *m_response;
    Api *m_api;

    QList<RequestParameter *> m_parameters;
    QQmlListProperty<RequestParameter> m_parametersProperty;
};

class Body : public QObject
{
    QML_ANONYMOUS
    QML_ADDED_IN_VERSION(2, 0)

    Q_OBJECT
    Q_PROPERTY(QVariant data READ data WRITE setData NOTIFY contentChanged FINAL)
    Q_PROPERTY(QString file READ fileName WRITE setFileName NOTIFY contentChanged FINAL)

public:
    enum Content {
        Data,
        File
    };

    explicit Body(QObject *parent = nullptr);

    QVariant data() const;
    void setData(const QVariant &data);

    QString fileName() const;
    void setFileName(const QString &fileName);

    RestLink::Body body() const;

signals:
    void contentChanged();

private:
    QVariant m_content;
    Content m_contentType;
};

} // namespace Qml
} // namespace RestLink

#endif // REQUEST_H
