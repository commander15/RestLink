#ifndef RESTLINK_RESPONSE_P_H
#define RESTLINK_RESPONSE_P_H

#include "response.h"

#include <RestLink/request.h>

namespace RestLink {

class ResponsePrivate
{
public:
    ResponsePrivate(Response *q);

    Response *q_ptr;

    Request request;
    Api *api;
    QByteArray body;
};

}

#endif // RESTLINK_RESPONSE_P_H
