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

void AbstractServerWorker::registerController(AbstractController *controller)
{
    QMutexLocker locker(&d_ptr->mutex);
    if (!d_ptr->controllers.contains(controller))
        d_ptr->controllers.append(controller);
}

void AbstractServerWorker::unregisterController(AbstractController *controller)
{
    QMutexLocker locker(&d_ptr->mutex);
    d_ptr->controllers.removeOne(controller);
}

void AbstractServerWorker::processInternalRequest(ServerRequest &request, ServerResponse *response)
{
    const QString function = request.endpoint().mid(1);

    // No functions for now
    Q_UNUSED(function)

    processUnsupportedRequest(request, response);
}

void AbstractServerWorker::processStandardRequest(ServerRequest &request, ServerResponse *response)
{
    d_ptr->mutex.lock();
    auto it = std::find_if(d_ptr->controllers.cbegin(), d_ptr->controllers.cend(), [&request](AbstractController *controller) {
        return controller->canProcessRequest(request);
    });
    AbstractController *controller = (it == d_ptr->controllers.cend() ? nullptr : *it);
    d_ptr->mutex.unlock();

    if (controller != nullptr) {
        processControllerRequest(request, response, controller);
        return;
    }

    processUnsupportedRequest(request, response);
}

void AbstractServerWorker::processUnsupportedRequest(const ServerRequest &request, ServerResponse *response)
{
    QString msg = QStringLiteral("unsupported method %1 for endpoint %2")
        .arg(HttpUtils::verbString(request.method()), request.endpoint());

    response->setHttpStatusCode(404);
    response->setBody(QJsonObject({ { "message", msg } }));
    response->complete();
}

void AbstractServerWorker::processControllerRequest(ServerRequest &request, ServerResponse *response, AbstractController *controller)
{
    void *source = createDataSource(request);
    controller->setDataSource(source);
    controller->processRequest(request, response);

    if (source == nullptr)
        return;

    switch (d_ptr->type) {
    case Synchronous:
        clearDataSource(request, source);
        break;

    case Asynchronous:
        connect(response, &Response::finished, this, [this, request, source] { clearDataSource(request, source); });
        break;
    }
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
    while (!controllers.isEmpty())
        delete controllers.takeLast();
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

    QObject::connect(&timer, &QTimer::timeout, q_ptr, [this] {
        if (q_ptr->isInterruptionRequested())
            return;

        if (processNext())
            return;

        if (!q_ptr->maintain())
            q_ptr->exit(1);
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
    }

    pending = pendingRequests.dequeue();
    if (pending.internal) {
        q_ptr->processInternalRequest(pending.request, pending.response);
    } else {
        q_ptr->processStandardRequest(pending.request, pending.response);
    }

    static const std::function<void(ServerResponse *)> autoComplete = [](ServerResponse *response) {
        // If not completed, we complete and assign a HTTP 500 status if no status has been set
        if (response->isFinished())
            return;

        if (response->httpStatusCode() == 0)
            response->setHttpStatusCode(500);

        response->complete();
    };

    // Process timeout
    switch (type) {
    case AbstractServerWorker::Synchronous:
        autoComplete(pending.response);
        break;

    case AbstractServerWorker::Asynchronous:
        QTimer::singleShot(pending.request.timeout(), pending.response, [pending] {
            autoComplete(pending.response);
        });
        break;
    }

    return true;
}

} // namespace RestLink
