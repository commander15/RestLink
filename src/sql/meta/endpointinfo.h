#ifndef ENDPOINTINFO_H
#define ENDPOINTINFO_H

#include "parseddata.h"
#include "sqlqueryinfo.h"
#include "resourceinfo.h"

#include <global.h>

namespace RestLink {
namespace Sql {

class EndpointInfoData;
class SQL_EXPORT EndpointInfo : public ParsedData
{
public:
    EndpointInfo();
    EndpointInfo(const EndpointInfo &other);
    ~EndpointInfo();

    EndpointInfo &operator=(const EndpointInfo &other);

    QString name() const;

    bool hasQuery() const;
    SqlQueryInfo query() const;

    bool hasResource() const;
    ResourceInfo resource() const;

    void load(const QString &name, const QJsonObject &object, Api *api);
    void save(QJsonObject *object) const;

    static EndpointInfo fromResource(const ResourceInfo &resource);

private:
    QExplicitlySharedDataPointer<EndpointInfoData> d_ptr;
};

} // namespace Sql
} // namespace RestLink

#endif // ENDPOINTINFO_H
