#ifndef RESTLINK_SERVERRESPONSE_P_H
#define RESTLINK_SERVERRESPONSE_P_H

#include "serverresponse.h"

#include <RestLink/header.h>
#include <RestLink/body.h>
#include <RestLink/private/response_p.h>

#include <QtCore/qjsonvalue.h>
#include <QtCore/qreadwritelock.h>

#include <QtNetwork/qnetworkrequest.h>

namespace RestLink {

class ServerResponsePrivate final : public ResponsePrivate
{
public:
    ServerResponsePrivate(ServerResponse *q);
    ~ServerResponsePrivate();

    Body readBody();

    AbstractRequestHandler::Method method;
    Body body;

    int httpStatusCode;
    HeaderList headers;

    bool finished;
    bool atEnd;

    QNetworkRequest networkRequest;

    Server *server;

    mutable QReadWriteLock lock;
};

} // namespace RestLink

#endif // RESTLINK_SERVERRESPONSE_P_H
