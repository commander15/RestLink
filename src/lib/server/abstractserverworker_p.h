#ifndef RESTLINK_ABSTRACTSERVERWORKER_P_H
#define RESTLINK_ABSTRACTSERVERWORKER_P_H

#include "abstractserverworker.h"

#include <QtCore/qqueue.h>
#include <QtCore/qmutex.h>

#include <RestLink/serverrequest.h>
#include <RestLink/serverresponse.h>

namespace RestLink {

class AbstractServerWorkerPrivate
{
public:
    struct PendingRequest {
        bool operator==(const PendingRequest &other) const
        { return response == other.response; }

        ServerRequest request;
        ServerResponse *response = nullptr;
        bool internal = false;
    };

    AbstractServerWorkerPrivate(AbstractServerWorker::WorkerType type, AbstractServerWorker *q);
    ~AbstractServerWorkerPrivate();

    void syncRun(int interval);
    void assyncRun(int interval);

    bool processNext();

    AbstractController *requestController(const PendingRequest &pending, bool *deletable = nullptr);

    AbstractServerWorker *q_ptr;

    QQueue<PendingRequest> pendingRequests;
    QList<class AbstractController *> controllers;
    AbstractServerWorker::WorkerType type;
    QMutex mutex;
};

}

#endif // RESTLINK_ABSTRACTSERVERWORKER_P_H
