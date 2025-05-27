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
            output.replace("{{" + name + "}}", data.value(name).toString());

        const QStringList parameterNames = parameters.keys();
        for (const QString &name : parameterNames)
            output.replace("{{" + name + "}}", parameters.value(name).toString());

        return output;
    }

    QStringList statements;
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

QString SqlQueryInfo::statement() const
{
    return d_ptr->statements.isEmpty() ? QString() : d_ptr->statements.first();
}

QString SqlQueryInfo::formated(const QVariantHash &data) const
{
    return (d_ptr->statements.isEmpty() ? QString() : d_ptr->format(d_ptr->statements.first(), data));
}

QStringList SqlQueryInfo::statements() const
{
    return d_ptr->statements;
}

QStringList SqlQueryInfo::allFormated(const QVariantHash &data) const
{
    QStringList queries = d_ptr->statements;

    std::transform(queries.begin(), queries.end(), queries.begin(), [this, &data](const QString &query) {
        return d_ptr->format(query, data);
    });

    return queries;
}

QVariant SqlQueryInfo::parameterValue(const QString &name) const
{
    return d_ptr->parameters.value(name);
}

QStringList SqlQueryInfo::parameterNames() const
{
    return d_ptr->parameters.keys();
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
    return !d_ptr->statements.isEmpty();
}

void SqlQueryInfo::load(const QJsonObject &object)
{
    auto generateStatements = [](const QJsonObject &object) -> QStringList {
        if (object.contains("statement"))
            return QStringList() << object.value("statement").toString();
        return QStringList();
    };

    beginParsing(object);
    attribute("statements", Callback<QStringList>(generateStatements), &d_ptr->statements);
    attribute("object", false, &d_ptr->object);
    attribute("parameters", &d_ptr->parameters);
    endParsing();
}

void SqlQueryInfo::save(QJsonObject *object) const
{
    object->insert("statements", QJsonValue::fromVariant(d_ptr->statements));
    object->insert("object", d_ptr->object);
    object->insert("parameters", QJsonValue::fromVariant(d_ptr->parameters));
}

} // namespace Sql
} // namespace RestLink
