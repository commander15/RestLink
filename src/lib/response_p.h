#ifndef RESTLINK_RESPONSE_P_H
#define RESTLINK_RESPONSE_P_H

#include "response.h"

#include <RestLink/request.h>

namespace RestLink {

class ResponsePrivate
{
public:
    ResponsePrivate(Response *q);
    virtual ~ResponsePrivate() = default;

    Response *q_ptr;

    Request request;
    QByteArray body;
};

}

#endif // RESTLINK_RESPONSE_P_H
