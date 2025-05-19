#include "sqlqueryinfo.h"

#include <QtCore/qvariant.h>
#include <QtCore/qjsonobject.h>

namespace RestLink {
namespace Sql {

class SqlQueryInfoData : public QSharedData
{
public:
    QString format(const QString &query, const QVariantHash &data) const
    {
        QString output = query;

        const QStringList dataNames = data.keys();
        for (const QString &name : dataNames)
            output.replace('{' + name + '}', data.value(name).toString());

        const QStringList parameterNames = parameters.keys();
        for (const QString &name : parameterNames)
            output.replace('{' + name + '}', parameters.value(name).toString());

        return output;
    }

    QStringList queries;
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
    return d_ptr->queries.isEmpty() ? QString() : d_ptr->queries.first();
}

QString SqlQueryInfo::formated(const QVariantHash &data) const
{
    return (d_ptr->queries.isEmpty() ? QString() : d_ptr->format(d_ptr->queries.first(), data));
}

QStringList SqlQueryInfo::allFormated(const QVariantHash &data) const
{
    QStringList queries = d_ptr->queries;

    std::transform(queries.begin(), queries.end(), queries.begin(), [this, &data](const QString &query) {
        return d_ptr->format(query, data);
    });

    return queries;
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
    return !d_ptr->queries.isEmpty();
}

void SqlQueryInfo::load(const QJsonObject &object)
{
    auto generateQueries = [](const QJsonObject &object) -> QStringList {
        if (object.contains("query"))
            return QStringList() << object.value("query").toString();

        if (object.contains("queries"))
            return object.value("queries").toVariant().toStringList();

        return QStringList();
    };

    beginParsing(object);
    attribute("queries", Callback<QStringList>(generateQueries), &d_ptr->queries);
    attribute("object", false, &d_ptr->object);
    attribute("parameters", &d_ptr->parameters);
    endParsing();
}

void SqlQueryInfo::save(QJsonObject *object) const
{
    object->insert("queries", QJsonValue::fromVariant(d_ptr->queries));
    object->insert("object", d_ptr->object);
    object->insert("parameters", QJsonValue::fromVariant(d_ptr->parameters));
}

} // namespace Sql
} // namespace RestLink
