#ifndef RESTLINK_SQLRESPONSE_P_H
#define RESTLINK_SQLRESPONSE_P_H

#include "sqlresponse.h"

#include <RestLink/private/internalresponse_p.h>

namespace RestLink {

class SqlResponsePrivate : public InternalResponsePrivate
{
public:
    SqlResponsePrivate(SqlResponse *response);

    QString query;
    bool singleResult;
};

} // namespace RestLink

#endif // RESTLINK_SQLRESPONSE_P_H
