#include "server.h"
#include "server_p.h"

#include <RestLink/serverrequest.h>
#include <RestLink/serverresponse.h>
#include <RestLink/abstractserverworker.h>

namespace RestLink {

Server::Server(AbstractServerWorker *worker, QObject *parent)
    : Server(new ServerPrivate(worker, this), parent)
{
}

Server::Server(ServerPrivate *d, QObject *parent)
    : QObject(parent)
    , d_ptr(d)
{
    d_ptr->worker->setParent(this);
}

Server::~Server()
{
    if (d_ptr->worker->isRunning()) {
        d_ptr->worker->requestInterruption();
        d_ptr->worker->wait();
    }
}

bool Server::isRunning() const
{
    return (d_ptr->worker ? d_ptr->worker->isRunning() : false);
}

void Server::start()
{
    if (d_ptr->worker)
        d_ptr->worker->start();
}

void Server::stop()
{
    if (d_ptr->worker)
        d_ptr->worker->requestInterruption();
}

Server *Server::create(const QString &name, const QStringList &schemes, AbstractServerWorker *worker, QObject *parent)
{
    return new DefaultServer(name, schemes, worker, parent);
}

AbstractRequestHandler::HandlerType Server::handlerType() const
{
    return ServerHandler;
}

Response *Server::sendRequest(Method method, const Request &request, const Body &body)
{
    const ServerRequest serverRequest(method, request, body);

    ServerResponse *serverResponse = new ServerResponse(this);
    initResponse(serverResponse, request, method);
    serverResponse->setMethod(method);

    if (!d_ptr->worker) {
        AbstractServerWorker::processUnsupportedRequest(serverRequest, serverResponse);
        return serverResponse;
    }

    d_ptr->worker->enqueue(serverRequest, serverResponse);
    if (!d_ptr->worker->isRunning())
        d_ptr->worker->start();

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
