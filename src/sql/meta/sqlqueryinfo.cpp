#include "sqlqueryinfo.h"

#include <QtCore/qvariant.h>
#include <QtCore/qjsonobject.h>

namespace RestLink {
namespace Sql {

class SqlQueryInfoData : public QSharedData
{
public:
    QString query;
    QVariantHash parameters;
    bool object = false;
};

SqlQueryInfo::SqlQueryInfo()
    : d_ptr(new SqlQueryInfoData())
{
}

SqlQueryInfo::SqlQueryInfo(const SqlQueryInfo &other)
    : d_ptr(other.d_ptr)
{
}

SqlQueryInfo::~SqlQueryInfo()
{
}

SqlQueryInfo &SqlQueryInfo::operator=(const SqlQueryInfo &other)
{
    if (this != &other)
        d_ptr = other.d_ptr;
    return *this;
}

QString SqlQueryInfo::query() const
{
    return d_ptr->query;
}

QString SqlQueryInfo::formated(const QVariantHash &data) const
{
    QString query = d_ptr->query;

    const QStringList dataNames = data.keys();
    for (const QString &name : dataNames)
        query.replace('{' + name + '}', data.value(name).toString());

    const QStringList parameterNames = d_ptr->parameters.keys();
    for (const QString &name : parameterNames)
        query.replace('{' + name + '}', d_ptr->parameters.value(name).toString());

    return query;
}

bool SqlQueryInfo::isObjectQuery() const
{
    return d_ptr->object;
}

bool SqlQueryInfo::isArrayQuery() const
{
    return !d_ptr->object;
}

bool SqlQueryInfo::isValid() const
{
    return !d_ptr->query.isEmpty();
}

void SqlQueryInfo::load(const QJsonObject &object)
{
    beginParsing(object);
    attribute("query", &d_ptr->query);
    attribute("object", false, &d_ptr->object);
    attribute("parameters", &d_ptr->parameters);
    endParsing();
}

void SqlQueryInfo::save(QJsonObject *object) const
{
    object->insert("query", d_ptr->query);
    object->insert("object", d_ptr->object);
}

} // namespace Sql
} // namespace RestLink
