#ifndef RESTLINK_SERVER_H
#define RESTLINK_SERVER_H

#include <RestLink/global.h>
#include <RestLink/abstractrequesthandler.h>

#include <QtCore/qobject.h>

namespace RestLink {

class ServerRequest;
class ServerResponse;
class AbstractServerWorker;

class ServerPrivate;
class RESTLINK_EXPORT Server : public QObject, public AbstractRequestHandler
{
    Q_OBJECT

public:
    virtual ~Server();

    bool isRunning() const;
    void start();
    void stop();

    HandlerType handlerType() const override;

    template<typename Worker>
    static Server *create(const QString &name, const QStringList &schemes, QObject *parent = nullptr)
    { return create(name, schemes, new Worker(), parent); }

    static Server *create(const QString &name, const QStringList &schemes, AbstractServerWorker *worker, QObject *parent = nullptr);

signals:
    void started();
    void stopped();

protected:
    Server(AbstractServerWorker *worker, QObject *parent = nullptr);
    Server(ServerPrivate *d, QObject *parent);

    Response *sendRequest(Method method, const Request &request, const Body &body) override final;

    QScopedPointer<ServerPrivate> d_ptr;

    friend class ServerPrivate;
};

} // namespace RestLink

#endif // RESTLINK_SERVER_H
