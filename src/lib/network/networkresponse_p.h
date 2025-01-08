#ifndef RESTLINK_NETWORKRESPONSE_P_H
#define RESTLINK_NETWORKRESPONSE_P_H

#include "networkresponse.h"

#include "../response_p.h"

namespace RestLink {

class NetworkResponsePrivate : public ResponsePrivate
{
public:
    NetworkResponsePrivate(Response *q);

    QNetworkReply *netReply;
};

} // namespace RestLink

#endif // RESTLINK_NETWORKRESPONSE_P_H
