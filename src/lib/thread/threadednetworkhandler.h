#ifndef RESTLINK_THREADEDNETWORKHANDLER_H
#define RESTLINK_THREADEDNETWORKHANDLER_H

#include <RestLink/global.h>
#include <RestLink/networkmanager.h>

#include <QtCore/qthread.h>
#include <QtCore/qqueue.h>
#include <QtCore/qmutex.h>

namespace RestLink {

class Response;

class RESTLINK_EXPORT ThreadedNetworkHandler : public QThread, public NetworkRequestHandler
{
    Q_OBJECT

public:
    enum HandlerType {
        Synchronous,
        Assynchronous
    };

    explicit ThreadedNetworkHandler(QObject *parent = nullptr);
    ThreadedNetworkHandler(HandlerType type, QObject *parent = nullptr);
    virtual ~ThreadedNetworkHandler();

    Response *send(ApiBase::Operation operation, const Request &request, const Body &body, Api *api) override final;

protected:
    bool processNext();
    virtual void process(Response *response) = 0;
    Response *nextPendingResponse();

    virtual Response *sendRequest(ApiBase::Operation operation, const Request &request, const Body &body, Api *api) = 0;

    virtual bool init() = 0;
    virtual void cleanup() = 0;
    void run() override final;

private:
    const HandlerType m_type;
    QQueue<Response *> m_responses;

    bool m_bypassCleanup;

    QMutex m_mutex;
};

} // namespace RestLink

#endif // RESTLINK_THREADEDNETWORKHANDLER_H
