#ifndef RESTLINK_APIBASE_P_H
#define RESTLINK_APIBASE_P_H

#include "apibase.h"

#include <RestLink/request.h>
#include <RestLink/requestinterceptor.h>

namespace RestLink {

class ApiBasePrivate
{
public:
    ApiBasePrivate(ApiBase *q);

    NetworkManager *networkManager() const;
    void setNetworkManager(NetworkManager *manager);

    static QByteArray httpVerbFromOperation(int op);

    ApiBase *q_ptr;

    Request internalRequest;
    QVector<RequestInterceptor *> requestInterceptors;

private:
    mutable NetworkManager *m_networkManager;
};

}

#endif // RESTLINK_APIBASE_P_H
