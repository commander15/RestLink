#ifndef RESTLINK_SERVER_H
#define RESTLINK_SERVER_H

#include <RestLink/global.h>
#include <RestLink/requesthandler.h>
#include <RestLink/serverrequest.h>
#include <RestLink/serverresponse.h>

#include <QtCore/qobject.h>

namespace RestLink {

class AbstractController;

class ServerPrivate;
class RESTLINK_EXPORT Server : public QObject, public RequestHandler
{
    Q_OBJECT

public:
    enum ServerType {
        Synchronous,
        Asynchronous
    };

    explicit Server(QObject *parent = nullptr);
    Server(ServerType type, QObject *parent = nullptr);
    virtual ~Server();

    bool isListening() const;
    void listen();
    void close();

    int error() const;
    QString errorString() const;

    void registerController(AbstractController *controller);

    HandlerType handlerType() const override;

signals:
    void stateChanged(bool listening);
    void errorOccured(int error);

protected:
    Server(ServerPrivate *d, QObject *parent);

    virtual bool init() = 0;
    virtual void cleanup() = 0;
    virtual bool maintain() = 0;

    ServerResponse *sendRequest(ApiBase::Operation operation, const Request &request, const Body &body) override final;
    virtual void processInternalRequest(ApiBase::Operation operation, const ServerRequest &request, ServerResponse *response);
    virtual void processRequest(ApiBase::Operation operation, const ServerRequest &request, ServerResponse *response);

    virtual AbstractController *findController(const ServerRequest &request) const;
    virtual void prepareController(AbstractController *controller, Api::Operation operation, const ServerRequest &request, ServerResponse *response);

    void setError(int code, const QString &str);

    QScopedPointer<ServerPrivate> d_ptr;

private:
    void setListening(bool listening);

    friend class ServerPrivate;
    friend class ResourceHandler;
};

} // namespace RestLink

#endif // RESTLINK_SERVER_H
