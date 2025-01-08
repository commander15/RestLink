#ifndef RESTLINK_APIBASE_P_H
#define RESTLINK_APIBASE_P_H

#include "apibase.h"

#include <RestLink/request.h>
#include <RestLink/requestinterceptor.h>

namespace RestLink {

class ApiReply;

class ApiBasePrivate
{
public:
    ApiBasePrivate(ApiBase *q);

    NetworkManager *netMan() const;
    void setNetMan(NetworkManager *man);

    static QByteArray httpVerbFromOperation(int op);

    ApiBase *q_ptr;

    Request internalRequest;
    QVector<RequestInterceptor *> requestInterceptors;

private:
    mutable NetworkManager *m_netMan;
};

}

#endif // RESTLINK_APIBASE_P_H
