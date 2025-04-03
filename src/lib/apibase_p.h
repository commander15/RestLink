#ifndef RESTLINK_APIBASE_P_H
#define RESTLINK_APIBASE_P_H

#include "apibase.h"

#include <RestLink/request.h>
#include <RestLink/requestinterceptor.h>

#include <RestLink/private/request_p.h>

namespace RestLink {

class ApiBasePrivate : public RequestData
{
public:
    ApiBasePrivate(ApiBase *q);
    ~ApiBasePrivate();

    NetworkManager *networkManager() const;
    void setNetworkManager(NetworkManager *manager);

    static QByteArray httpVerbFromOperation(int op);

    ApiBase *q_ptr;

    RequestData *internalRequestData;
    QVector<RequestInterceptor *> requestInterceptors;

private:
    mutable NetworkManager *m_networkManager;
};

}

#endif // RESTLINK_APIBASE_P_H
