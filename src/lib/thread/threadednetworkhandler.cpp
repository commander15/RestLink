#include "threadednetworkhandler.h"
#include "threadednetworkhandler_p.h"

#include <QtCore/qtimer.h>

namespace RestLink {

ThreadedNetworkHandler::ThreadedNetworkHandler(QObject *parent)
    : ThreadedNetworkHandler(Assynchronous, parent)
{
}

ThreadedNetworkHandler::ThreadedNetworkHandler(HandlerType type, QObject *parent)
    : QThread(parent)
    , m_type(type),
    m_bypassCleanup(false)
{
    setTerminationEnabled(false);
}

ThreadedNetworkHandler::~ThreadedNetworkHandler()
{
    if (isRunning()) {
        m_mutex.lock();
        m_bypassCleanup = true;
        m_mutex.unlock();

        requestInterruption();
        wait();
    }
}

Response *ThreadedNetworkHandler::send(ApiBase::Operation operation, const Request &request, const Body &body, Api *api)
{
    Response *response = sendRequest(operation, request, body, api);
    m_mutex.lock();
    m_responses.enqueue(response);
    m_mutex.unlock();

    if (!isRunning())
        start();

    return response;
}

bool ThreadedNetworkHandler::processNext()
{
    Response *response = nextPendingResponse();
    if (response) {
        process(response);
        return true;
    } else {
        return false;
    }
}

Response *ThreadedNetworkHandler::nextPendingResponse()
{
    QMutexLocker locker(&m_mutex);
    return (!m_responses.isEmpty() ? m_responses.dequeue() : nullptr);
}

void ThreadedNetworkHandler::run()
{
    const int interval = 250;

    if (m_type == Synchronous) {
        init();
        while (!isInterruptionRequested()) {
            processNext();
            msleep(interval);
        }
    } else if (m_type == Assynchronous) {
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
            init();

            // Starting loop timer
            timer.start();
        });

        // Starting event loop
        exec();
    }

    // Calling cleanup function
    if (!m_bypassCleanup)
        cleanup();
}

} // namespace RestLink
