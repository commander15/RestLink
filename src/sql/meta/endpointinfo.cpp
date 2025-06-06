#include "endpointinfo.h"

#include <api.h>

#include <QtCore/qjsonobject.h>

namespace RestLink {
namespace Sql {

class EndpointInfoData : public QSharedData
{
public:
    QString name;
    SqlQueryInfo getQuery;
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

bool EndpointInfo::hasGetQuery() const
{
    return d_ptr->getQuery.isValid();
}

SqlQueryInfo EndpointInfo::getQuery() const
{
    return d_ptr->getQuery;
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

    if (object.contains("get_query"))
        d_ptr->getQuery.load(object.value("get_query").toObject());

    if (object.contains("resource")) {
        const QString resourceName = object.value("resource").toString();
        d_ptr->resource = api->resourceInfo(resourceName);
    }
}

void EndpointInfo::save(QJsonObject *object) const
{
    if (d_ptr->getQuery.isValid()) {
        QJsonObject getQuery;
        d_ptr->getQuery.save(&getQuery);
        object->insert("get_query", getQuery);
    }

    if (d_ptr->resource.isValid())
        object->insert("resource", d_ptr->resource.name());
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
