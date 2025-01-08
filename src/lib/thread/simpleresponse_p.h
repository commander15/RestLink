#ifndef RESTLINK_SIMPLERESPONSE_P_H
#define RESTLINK_SIMPLERESPONSE_P_H

#include "simpleresponse.h"

#include "../response_p.h"

#include <RestLink/header.h>

#include <QtCore/qmutex.h>

namespace RestLink {

class SimpleResponsePrivate : public ResponsePrivate
{
public:
    SimpleResponsePrivate(SimpleResponse *q);

    QByteArray data;
    QVector<Header> headers;

    mutable QMutex mutex;
};

} // namespace RestLink

#endif // RESTLINK_SIMPLERESPONSE_P_H
