#ifndef RESTLINK_SERVERRESPONSE_P_H
#define RESTLINK_SERVERRESPONSE_P_H

#include "serverresponse.h"

#include <RestLink/header.h>
#include <RestLink/body.h>
#include <RestLink/private/response_p.h>

#include <QtCore/qjsonvalue.h>
#include <QtCore/qmutex.h>

#include <QtNetwork/qnetworkrequest.h>

namespace RestLink {

class ServerResponsePrivate final : public ResponsePrivate
{
public:
    ServerResponsePrivate(ServerResponse *q);

    Body readBody();

    AbstractRequestHandler::Method method;

    int httpStatusCode;
    Body body;
    HeaderList headers;

    bool finished;
    bool atEnd;

    QNetworkRequest networkRequest;

    Server *server;

    mutable QMutex mutex;
};

} // namespace RestLink

#endif // RESTLINK_SERVERRESPONSE_P_H
