#include "sqlresponse.h"
#include "sqlresponse_p.h"

namespace RestLink {

SqlResponse::SqlResponse(Api *api)
    : InternalResponse(new SqlResponsePrivate(this), api)
{
}

RestLink::SqlResponse::~SqlResponse()
{
}

QString SqlResponse::query() const
{
    RESTLINK_D(const SqlResponse);
    return d->query;
}

bool SqlResponse::isSingleResult() const
{
    RESTLINK_D(const SqlResponse);
    return d->singleResult;
}

void SqlResponse::setQuery(const QString &query, bool singleResult)
{
    RESTLINK_D(SqlResponse);
    d->query = query;
    d->singleResult = singleResult;
}

SqlResponsePrivate::SqlResponsePrivate(SqlResponse *response)
    : InternalResponsePrivate(response)
    , singleResult(false)
{
}

} // namespace RestLink
