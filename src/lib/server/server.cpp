#include "server.h"
#include "server_p.h"

#include <QtCore/qtimer.h>

#include <RestLink/serverrequest.h>
#include <RestLink/serverresponse.h>
#include <RestLink/abstractserverworker.h>
#include <qcoreapplication.h>

namespace RestLink {

Server::Server(AbstractServerWorker *worker, QObject *parent)
    : Server(new ServerPrivate(worker, this), parent)
{
}

Server::Server(ServerPrivate *d, QObject *parent)
    : QObject(parent)
    , d_ptr(d)
{
    d->worker->setParent(this);
    connect(qApp, &QCoreApplication::aboutToQuit, this, &Server::stop);
    connect(qApp, &QCoreApplication::aboutToQuit, this, &Server::wait);
}

Server::~Server()
{
}

bool Server::isRunning() const
{
    return d_ptr->worker->isRunning();
}

void Server::start()
{
    if (!d_ptr->worker->isRunning())
        d_ptr->worker->start();
}

void Server::stop()
{
    if (d_ptr->worker->isRunning()) {
        d_ptr->worker->requestInterruption();
        d_ptr->worker->quit();
    }
}

void Server::wait()
{
    if (d_ptr->worker->isRunning())
        d_ptr->worker->wait();
}

QByteArray Server::handlerId() const
{
    return QByteArrayLiteral("restlink.server");
}

Server *Server::create(const QString &name, const QStringList &schemes, AbstractServerWorker *worker, QObject *parent)
{
    QString id = name.toLower();
    id.replace(' ', '.');
    return new DefaultServer(id.toLower().toUtf8(), name, schemes, worker, parent);
}

Server *Server::create(const QByteArray &id, const QString &name, const QStringList &schemes, AbstractServerWorker *worker, QObject *parent)
{
    return new DefaultServer(id, name, schemes, worker, parent);
}

AbstractRequestHandler::HandlerType Server::handlerType() const
{
    return HandlerType::ServerHandler;
}

Response *Server::sendRequest(Method method, const Request &request, const Body &body)
{
    const ServerRequest serverRequest(method, request, body);

    ServerResponse *serverResponse = new ServerResponse(this);
    initResponse(serverResponse, request, method);
    serverResponse->setMethod(method);

    d_ptr->worker->enqueue(serverRequest, serverResponse);
    if (!d_ptr->worker->isRunning()) {
        d_ptr->worker->start();
    }

    return serverResponse;
}

ServerPrivate::ServerPrivate(AbstractServerWorker *worker, Server *q)
    : q_ptr(q)
    , worker(worker)
{
}

ServerPrivate::~ServerPrivate()
{
    if (worker)
        delete worker;
}

} // namespace RestLink
