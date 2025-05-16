#include "endpointinfo.h"

#include <api.h>

#include <QtCore/qjsonobject.h>

namespace RestLink {
namespace Sql {

class EndpointInfoData : public QSharedData
{
public:
    QString name;
    SqlQueryInfo query;
    ResourceInfo resource;
};

EndpointInfo::EndpointInfo()
    : d_ptr(new EndpointInfoData())
{
}

EndpointInfo::EndpointInfo(const EndpointInfo &other)
    : d_ptr(other.d_ptr)
{
}

EndpointInfo::~EndpointInfo()
{
}

EndpointInfo &EndpointInfo::operator=(const EndpointInfo &other)
{
    if (this != &other)
        d_ptr = other.d_ptr;
    return *this;
}

QString EndpointInfo::name() const
{
    return d_ptr->name;
}

bool EndpointInfo::hasQuery() const
{
    return d_ptr->query.isValid();
}

SqlQueryInfo EndpointInfo::query() const
{
    return d_ptr->query;
}

bool EndpointInfo::hasResource() const
{
    return d_ptr->resource.isValid();
}

ResourceInfo EndpointInfo::resource() const
{
    return d_ptr->resource;
}

void EndpointInfo::load(const QString &name, const QJsonObject &object, Api *api)
{
    d_ptr->name = name;

    if (object.contains("query"))
        d_ptr->query.load(object);

    if (object.contains("resource")) {
        const QString resourceName = object.value("resource").toString();
        d_ptr->resource = api->resourceInfo(resourceName);
    }
}

void EndpointInfo::save(QJsonObject *object) const
{
    if (d_ptr->query.isValid())
        d_ptr->query.save(object);

    if (d_ptr->resource.isValid())
        d_ptr->resource.save(object);
}

EndpointInfo EndpointInfo::fromResource(const ResourceInfo &resource)
{
    EndpointInfo endpoint;

    EndpointInfoData *data = endpoint.d_ptr.get();
    data->name = '/' + resource.name();
    data->resource = resource;

    return endpoint;
}

} // namespace Sql
} // namespace RestLink
