#ifndef RESTLINK_INTERNALRESPONSE_P_H
#define RESTLINK_INTERNALRESPONSE_P_H

#include "internalresponse.h"
#include "../response_p.h"

#include <RestLink/header.h>

#include <QtCore/qjsonvalue.h>
#include <QtCore/qmutex.h>

#include <QtNetwork/qnetworkrequest.h>

namespace RestLink {

class InternalResponsePrivate : public ResponsePrivate
{
public:
    InternalResponsePrivate(InternalResponse *q);

    Api::Operation operation;

    int httpStatusCode;
    QJsonValue data;
    QVector<Header> headers;

    bool finished;

    QNetworkRequest request;

    mutable QMutex mutex;
};

} // namespace RestLink

#endif // RESTLINK_INTERNALRESPONSE_P_H
