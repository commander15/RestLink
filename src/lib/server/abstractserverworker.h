#ifndef RESTLINK_ABSTRACTSERVERWORKER_H
#define RESTLINK_ABSTRACTSERVERWORKER_H

#include <RestLink/global.h>

#include <QtCore/qthread.h>

namespace RestLink {

class ServerRequest;
class ServerResponse;

class AbstractServerWorkerPrivate;
class RESTLINK_EXPORT AbstractServerWorker : public QThread
{
    Q_OBJECT

public:
    enum WorkerType {
        Synchronous,
        Asynchronous
    };

    AbstractServerWorker(WorkerType type, QObject *parent = nullptr);
    virtual ~AbstractServerWorker();

    void enqueue(const ServerRequest &request, ServerResponse *response);

    virtual void processInternalRequest(const ServerRequest &request, ServerResponse *response);
    virtual void processStandardRequest(const ServerRequest &request, ServerResponse *response) = 0;
    static void processUnsupportedRequest(const ServerRequest &request, ServerResponse *response);

protected:
    virtual bool init() = 0;
    virtual void cleanup() = 0;
    virtual bool maintain() = 0;

    virtual void *requestDataSource(const ServerRequest &request) = 0;
    virtual void clearDataSource(const ServerRequest &request, void *source) = 0;

    void run() override final;

private:
    QScopedPointer<AbstractServerWorkerPrivate> d_ptr;

    friend class AbstractServerWorkerPrivate;
};

} // namespace RestLink

#endif // RESTLINK_ABSTRACTSERVERWORKER_H
