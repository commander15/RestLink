#ifndef RESTLINK_APIBASE_P_H
#define RESTLINK_APIBASE_P_H

#include "apibase.h"

#include <RestLink/request.h>

#include <RestLink/private/request_p.h>

namespace RestLink {

class ApiBasePrivate : public RequestPrivate
{
public:
    ApiBasePrivate(ApiBase *q);
    ~ApiBasePrivate();

    NetworkManager *networkManager() const;
    void setNetworkManager(NetworkManager *manager);

    ApiBase *q_ptr;

    RequestPrivate *internalRequestData;

private:
    mutable NetworkManager *m_networkManager;
};

}

#endif // RESTLINK_APIBASE_P_H
