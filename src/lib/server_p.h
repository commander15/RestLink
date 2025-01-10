#ifndef RESTLINK_SERVER_P_H
#define RESTLINK_SERVER_P_H

#include "server.h"

#include <RestLink/request.h>
#include <RestLink/body.h>

#include <QtCore/qthread.h>
#include <QtCore/qqueue.h>
#include <QtCore/qmutex.h>

namespace RestLink {

class ServerPrivate : public QThread
{
    Q_OBJECT

public:
    struct PendingRequest {
        Api::Operation operation;
        Request request;
        Body body;
        Response *response = nullptr;
    };

    ServerPrivate(Server::ServerType type, Server *q);

    bool processNext();

    Server *q_ptr;

    bool listening;

    int errorCode;
    QString errorString;

    const Server::ServerType type;
    QQueue<PendingRequest> pendingRequests;
    bool bypassCleanup;

    mutable QMutex mutex;

protected:
    void run() override;

    friend class Server;
};

} // namespace RestLink

#endif // RESTLINK_SERVER_P_H
