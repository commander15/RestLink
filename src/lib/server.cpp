#include "server.h"
#include "server_p.h"

#include <RestLink/serverresponse.h>

#include <QtCore/qtimer.h>

#include <QtNetwork/qnetworkrequest.h>

namespace RestLink {

Server::Server(QObject *parent)
    : Server(Asynchronous, parent)
{
}

Server::Server(ServerType type, QObject *parent)
    : QObject{parent}
    , d_ptr(new ServerPrivate(type, this))
{
}

Server::Server(ServerPrivate *d, QObject *parent)
    : QObject(parent)
    , d_ptr(d)
{
}

Server::~Server()
{
    d_ptr->q_ptr = nullptr;

    if (d_ptr->isRunning()) {
        d_ptr->bypassCleanup = true;
        d_ptr->requestInterruption();
        d_ptr->wait();
    }
}

bool Server::isListening() const
{
    QMutexLocker locker(&d_ptr->mutex);
    return d_ptr->listening;
}

void Server::listen()
{
    if (!d_ptr->isRunning())
        d_ptr->start();
}

void Server::close()
{
    if (d_ptr->isRunning())
        d_ptr->requestInterruption();
}

int Server::error() const
{
    QMutexLocker locker(&d_ptr->mutex);
    return d_ptr->errorCode;
}

QString Server::errorString() const
{
    QMutexLocker locker(&d_ptr->mutex);
    return d_ptr->errorString;
}

Response *Server::createResponse(ApiBase::Operation operation, const Request &request, const Body &body, Api *api)
{
    ServerResponse *response = new ServerResponse(api);
    response->setNetworkRequest(QNetworkRequest(generateUrl(request, api)));
    return response;
}

void Server::setError(int code, const QString &str)
{
    d_ptr->mutex.lock();
    d_ptr->errorCode = code;
    d_ptr->errorString = str;
    d_ptr->mutex.unlock();

    if (code != 0)
        emit errorOccured(code);
}

void Server::setListening(bool listening)
{
    d_ptr->mutex.lock();
    d_ptr->listening = listening;
    d_ptr->mutex.unlock();

    emit stateChanged(listening);
}

Response *Server::sendRequest(ApiBase::Operation operation, const Request &request, const Body &body, Api *api)
{
    ServerPrivate::PendingRequest pending;
    pending.operation = operation;
    pending.request = request;
    pending.response = createResponse(operation, request, body, api);

    d_ptr->mutex.lock();
    d_ptr->pendingRequests.enqueue(pending);
    d_ptr->mutex.unlock();

    return pending.response;
}

ServerPrivate::ServerPrivate(Server::ServerType type, Server *q)
    : q_ptr(q)
    , listening(false)
    , type(type)
    , bypassCleanup(false)
{
    setTerminationEnabled(false);
}

bool ServerPrivate::processNext()
{
    PendingRequest pending;
    mutex.lock();
    if (!pendingRequests.isEmpty())
        pending = pendingRequests.dequeue();
    mutex.unlock();

    if (pending.response) {
        q_ptr->processRequest(pending.operation, pending.request, pending.body, pending.response);
        return true;
    } else {
        return false;
    }
}

void ServerPrivate::run()
{
    const int interval = 250;

    if (type == Server::Synchronous) {
        if (q_ptr->init()) {
            q_ptr->setListening(true);
        } else {
            q_ptr->setListening(false);
            return;
        }

        while (!isInterruptionRequested()) {
            if (!processNext())
                q_ptr->setListening(q_ptr->maintain());
            msleep(interval);
        }
    } else if (type == Server::Asynchronous) {
        QTimer timer;
        timer.setInterval(interval);
        connect(&timer, &QTimer::timeout, &timer, [this] {
            if (!isInterruptionRequested())
                processNext();
            else
                quit();
        });

        // Calling init function
        QTimer::singleShot(0, &timer, [this, &timer] {
            if (q_ptr->init()) {
                q_ptr->setListening(true);

                // Starting loop timer
                timer.start();
            } else {
                q_ptr->setListening(false);
                exit(1);
            }
        });

        // Starting event loop
        if (exec() != 0)
            bypassCleanup = true;
    }

    // Calling cleanup function
    if (!bypassCleanup)
        q_ptr->cleanup();
}

} // namespace RestLink
