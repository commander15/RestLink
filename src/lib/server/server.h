#ifndef RESTLINK_SERVER_H
#define RESTLINK_SERVER_H

#include <RestLink/global.h>
#include <RestLink/requesthandler.h>

#include <QtCore/qobject.h>

namespace RestLink {

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

    HandlerType handlerType() const override;

signals:
    void stateChanged(bool listening);
    void errorOccured(int error);

protected:
    Server(ServerPrivate *d, QObject *parent);

    virtual bool init() = 0;
    virtual void cleanup() = 0;
    virtual bool maintain() = 0;

    virtual void processRequest(ApiBase::Operation operation, const Request &request, const Body &body, Response *response) = 0;
    virtual Response *createResponse(ApiBase::Operation operation, const Request &request, const Body &body, Api *api);
    Response *sendRequest(ApiBase::Operation operation, const Request &request, const Body &body) override final;

    void setError(int code, const QString &str);

    QScopedPointer<ServerPrivate> d_ptr;

private:
    void setListening(bool listening);

    friend class ServerPrivate;
};

} // namespace RestLink

#endif // RESTLINK_SERVER_H
