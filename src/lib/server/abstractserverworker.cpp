#include "abstractserverworker.h"
#include "abstractserverworker_p.h"

#include <QtCore/qjsonobject.h>
#include <QtCore/qtimer.h>

#include <RestLink/abstractcontroller.h>
#include <RestLink/httputils.h>

namespace RestLink {

AbstractServerWorker::AbstractServerWorker(WorkerType type, QObject *parent)
    : QThread(parent)
    , d_ptr(new AbstractServerWorkerPrivate(type, this))
{
    setTerminationEnabled(false);
}

AbstractServerWorker::~AbstractServerWorker()
{
}

bool AbstractServerWorker::hasPendingRequests() const
{
    QMutexLocker locker(&d_ptr->mutex);
    return !d_ptr->pendingRequests.isEmpty();
}

void AbstractServerWorker::enqueue(const ServerRequest &request, ServerResponse *response)
{
    static const QStringList internals = {
        "/register-controller"
    };

    AbstractServerWorkerPrivate::PendingRequest pending;
    pending.request = request;
    pending.response = response;
    pending.internal = internals.contains(request.endpoint());

    d_ptr->mutex.lock();
    d_ptr->pendingRequests.enqueue(pending);
    d_ptr->mutex.unlock();

    connect(response, &QObject::destroyed, this, [this, pending](QObject *) {
        d_ptr->mutex.lock();
        if (d_ptr->pendingRequests.contains(pending))
            d_ptr->pendingRequests.removeOne(pending);
        d_ptr->mutex.unlock();
    });
}

void AbstractServerWorker::processInternalRequest(const ServerRequest &request, ServerResponse *response)
{
    const QString function = request.endpoint().mid(1);

    if (function == "register-controller") {
        AbstractController *controller = request.controller();
        bool registered = false;

        d_ptr->mutex.lock();
        if (controller && !d_ptr->controllers.contains(controller)) {
            d_ptr->controllers.prepend(controller);
            registered = true;
        }
        d_ptr->mutex.unlock();

        if (registered) {
            response->setHttpStatusCode(201);
            response->setBody(QJsonObject({ { "message", "controller registered !" } }));
        } else {
            response->setHttpStatusCode(400);
            response->setBody(QJsonObject({ { "message", "controller invalid or already registered !" } }));
        }

        response->complete();
        return;
    }

    processUnsupportedRequest(request, response);
}

void AbstractServerWorker::processUnsupportedRequest(const ServerRequest &request, ServerResponse *response)
{
    QString msg = QStringLiteral("unsupported method %1 for endpoint %2")
        .arg(HttpUtils::verbString(request.method()), request.endpoint());

    response->setHttpStatusCode(400);
    response->setBody(QJsonObject({ { "message", msg } }));
    response->complete();
}

void AbstractServerWorker::run()
{
    if (!init())
        return;

    const int interval = 250;

    switch (d_ptr->type) {
    case Synchronous:
        d_ptr->syncRun(interval);
        break;

    case Asynchronous:
        d_ptr->assyncRun(interval);
        break;

    default:
        break;
    }

    cleanup();
}

AbstractServerWorkerPrivate::AbstractServerWorkerPrivate(AbstractServerWorker::WorkerType type, AbstractServerWorker *q)
    : q_ptr(q)
    , type(type)
{
}

AbstractServerWorkerPrivate::~AbstractServerWorkerPrivate()
{
    QMutexLocker locker(&mutex);

    while (!pendingRequests.isEmpty()) {
        AbstractController *controller = pendingRequests.dequeue().request.controller();
        if (controller)
            delete controller;
    }

    while (!controllers.isEmpty())
        delete controllers.takeFirst();
}

void AbstractServerWorkerPrivate::syncRun(int interval)
{
    while (!q_ptr->isInterruptionRequested()) {
        if (!processNext())
            if (!q_ptr->maintain())
                return;
        q_ptr->msleep(interval);
    }
}

void AbstractServerWorkerPrivate::assyncRun(int interval)
{
    QTimer timer;
    timer.start(interval);

    QObject::connect(&timer, &QTimer::timeout, &timer, [this] {
        if (!q_ptr->isInterruptionRequested()) {
            if (!processNext())
                if (!q_ptr->maintain())
                    q_ptr->quit();
        } else {
            q_ptr->quit();
        }
    });

    q_ptr->exec();
}

bool AbstractServerWorkerPrivate::processNext()
{
    PendingRequest pending;

    {
        QMutexLocker locker(&mutex);
        if (pendingRequests.isEmpty())
            return false;
        else
            pending = pendingRequests.dequeue();
    }

    if (pending.internal) {
        q_ptr->processInternalRequest(pending.request, pending.response);
    } else {
        bool deletable = true;
        AbstractController *controller = requestController(pending, &deletable);
        if (controller) {
            void *source = q_ptr->createDataSource(pending.request);
            controller->setDataSource(source);
            if (controller->canProcessRequest(pending.request))
                controller->processRequest(pending.request, pending.response);
            q_ptr->clearDataSource(pending.request, source);

            if (deletable)
                delete controller;
        } else {
            q_ptr->processStandardRequest(pending.request, pending.response);
        }
    }

    if (pending.response->isRunning())
        pending.response->complete();

    return true;
}

AbstractController *AbstractServerWorkerPrivate::requestController(const PendingRequest &pending, bool *deletable)
{
    AbstractController *controller = pending.request.controller();
    if (controller != nullptr) {
        if (deletable) *deletable = true;
        return controller;
    }

    auto it = std::find_if(controllers.begin(), controllers.end(), [&pending](AbstractController *controller) {
        return controller->canProcessRequest(pending.request);
    });

    if (it != controllers.end()) {
        if (deletable) *deletable = false;
        return *it;
    }

    return nullptr;
}

} // namespace RestLink
