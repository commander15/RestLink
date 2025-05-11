#ifndef RESTLINK_BODY_P_H
#define RESTLINK_BODY_P_H

#include "body.h"

namespace RestLink {

class BodyData : public QSharedData
{
public:
    BodyData() = default;
    BodyData(const BodyData &other) = default;
    ~BodyData() = default;

    QVariant object;
    Body::Type objectType = Body::Type::Unknown;
    QByteArray contentType;
    HeaderList headers;
};

}

#endif // RESTLINK_BODY_P_H
