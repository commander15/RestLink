#ifndef RESTLINK_ABSTRACTREQUESTHANDLER_P_H
#define RESTLINK_ABSTRACTREQUESTHANDLER_P_H

#include "abstractrequesthandler.h"

#include <QtCore/qlist.h>

namespace RestLink {

class AbstractRequestHandlerPrivate
{
public:
    virtual ~AbstractRequestHandlerPrivate() = default;

    QList<AbstractRequestInterceptor *> interceptors;
};

}

#endif // RESTLINK_ABSTRACTREQUESTHANDLER_P_H
