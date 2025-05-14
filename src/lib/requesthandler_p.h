#ifndef RESTLINK_REQUESTHANDLER_P_H
#define RESTLINK_REQUESTHANDLER_P_H

#include "requesthandler.h"

#include <QtCore/qlist.h>

namespace RestLink {

class RequestHandlerPrivate
{
public:
    virtual ~RequestHandlerPrivate() = default;

    QList<RequestInterceptor *> interceptors;
};

}

#endif // RESTLINK_REQUESTHANDLER_P_H
