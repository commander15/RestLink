#ifndef RESTLINK_APIREPLY_P_H
#define RESTLINK_APIREPLY_P_H

#include "apireply.h"

#include <RestLink/apirequest.h>

namespace RestLink {

class ApiReplyPrivate
{
public:
    ApiReplyPrivate(ApiReply *qq);

    ApiReply *q;

    ApiRequest apiRequest;
    Api *api;

    QNetworkReply *netReply;
    QByteArray netData;

    QByteArray defaultData;
};

}

#endif // RESTLINK_APIREPLY_P_H
