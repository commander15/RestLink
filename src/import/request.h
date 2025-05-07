#ifndef REQUEST_H
#define REQUEST_H

#include <RestLink/parameterlist.h>
#include <RestLink/response.h>

#include <QtCore/qobject.h>

#include <QtQml/qqmlengine.h>

namespace RestLink {

class RequestInterface;
class Response;

namespace Qml {

class Request : public QObject
{
    QML_ELEMENT
    QML_ADDED_IN_VERSION(1, 0)

    Q_OBJECT
    Q_PROPERTY(int operation MEMBER m_operation NOTIFY operationChanged)
    Q_PROPERTY(QString endpoint MEMBER m_endpoint NOTIFY endpointChanged)
    Q_PROPERTY(QVariant body MEMBER m_body NOTIFY bodyChanged)
    Q_PROPERTY(bool running READ isRunning NOTIFY finished)
    Q_PROPERTY(bool finished READ isFinished NOTIFY finished)
    Q_PROPERTY(RestLink::Qml::Api* api MEMBER m_api NOTIFY apiChanged)

public:
    enum Operation {
        Get = Api::GetOperation,
        Post = Api::PostOperation,
        Put = Api::PutOperation,
        Patch = Api::PatchOperation,
        Delete = Api::DeleteOperation
    };
    Q_ENUM(Operation)

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

    void finished(RestLink::Response *response);

private:
    int m_operation;
    QString m_endpoint;
    QVariant m_body;

    Response *m_response;
    Api *m_api;
};

} // namespace Qml
} // namespace RestLink

#endif // REQUEST_H
