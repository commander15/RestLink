#include "server.h"
#include "server_p.h"

#include <QtCore/qtimer.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>

#include <QtNetwork/qnetworkrequest.h>

#include <RestLink/serverrequest.h>
#include <RestLink/serverresponse.h>
#include <RestLink/abstractcontroller.h>
#include <RestLink/httputils.h>

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

    while (!d_ptr->controllers.isEmpty())
        delete d_ptr->controllers.takeFirst();
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

QJsonObject Server::configuration() const
{
    return d_ptr->configuration;
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

void Server::registerController(AbstractController *controller)
{

}

RequestHandler::HandlerType Server::handlerType() const
{
    return ServerHandler;
}

ServerResponse *Server::sendRequest(Method method, const Request &request, const Body &body)
{
    const ServerRequest serverRequest(method, request, body);

    ServerResponse *serverResponse = new ServerResponse(this);
    initResponse(serverResponse, request, method);
    serverResponse->setMethod(method);
    serverResponse->setNetworkRequest(QNetworkRequest(request.url()));

    if (request.endpoint().startsWith("/restlink")) {
        processInternalRequest(serverRequest, serverResponse);
        return serverResponse;
    }

    ServerPrivate::PendingRequest pending;
    pending.method = method;
    pending.request = request;
    pending.body = body;
    pending.response = serverResponse;

    d_ptr->mutex.lock();
    d_ptr->pendingRequests.enqueue(pending);
    d_ptr->mutex.unlock();

    if (d_ptr->autoStart && !isListening())
        listen();

    return serverResponse;
}

void Server::processInternalRequest(const ServerRequest &request, ServerResponse *response)
{
    const QString function = request.endpoint().section('/', 1, -1, QString::SectionFlag::SectionSkipEmpty);

    if (function == "configuration") {
        switch (request.method()) {
        case RequestHandler::GetMethod:
            response->setBody(d_ptr->configuration);
            response->setHttpStatusCode(200);
            break;

        case RequestHandler::PostMethod:
        case RequestHandler::PutMethod:
        case RequestHandler::PatchMethod:
            d_ptr->configuration = QJsonDocument::fromJson(request.body().toByteArray()).object();
            response->setBody(request.body());
            response->setHttpStatusCode(200);
            break;

        case RequestHandler::DeleteMethod:
            d_ptr->configuration = QJsonObject();
            response->setBody(QJsonObject({ {"message", "configuration reseted successfully !"} }));
            response->setHttpStatusCode(200);

        default:
            break;
        }

        response->complete();
        return;
    }

    if (function == "register-controller") {
        AbstractController *controller = request.controller();
        if (controller && !d_ptr->controllers.contains(controller)) {
            d_ptr->controllers.append(controller);
            response->setHttpStatusCode(200);
        } else {
            response->setHttpStatusCode(403);
        }

        QTimer::singleShot(0, response, &ServerResponse::complete);
        return;
    }
}

void Server::processRequest(const ServerRequest &request, ServerResponse *response)
{
    class AbstractController *controller = findController(request);

    if (controller) {
        prepareController(controller, request, response);
        controller->processRequest(request, response);
    }
}

void Server::processUnsupportedRequest(const ServerRequest &request, ServerResponse *response)
{
    QString msg = QStringLiteral("unsupported method %1 for endpoint %2")
    .arg(HttpUtils::verbString(request.method()), request.endpoint());

    response->setHttpStatusCode(400);
    response->setBody(QJsonObject({ { "message", msg } }));
    response->complete();
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

AbstractController *Server::findController(const ServerRequest &request) const
{
    auto it = std::find_if(d_ptr->controllers.begin(), d_ptr->controllers.end(), [&request](class AbstractController *handler) {
        return handler->canProcessRequest(request);
    });

    if (it != d_ptr->controllers.end())
        return *it;

    return nullptr;
}

void Server::prepareController(AbstractController *controller, const ServerRequest &request, ServerResponse *response)
{
    Q_UNUSED(controller);
    Q_UNUSED(request);
    Q_UNUSED(response);
}

void Server::setListening(bool listening)
{
    d_ptr->mutex.lock();
    d_ptr->listening = listening;
    d_ptr->mutex.unlock();

    emit stateChanged(listening);
}

ServerPrivate::ServerPrivate(Server::ServerType type, Server *q)
    : q_ptr(q)
    , listening(false)
    , autoStart(true)
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
        q_ptr->processRequest(ServerRequest(pending.method, pending.request, pending.body), pending.response);
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
        if (exec() != 0) {
            mutex.lock();
            bypassCleanup = true;
            mutex.unlock();
        }
    }

    // Calling cleanup function
    mutex.lock();
    const bool clean = !bypassCleanup;
    mutex.unlock();

    if (clean)
        q_ptr->cleanup();
}

} // namespace RestLink
