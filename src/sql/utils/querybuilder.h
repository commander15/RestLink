#ifndef QUERYBUILDER_H
#define QUERYBUILDER_H

#include <global.h>

#include <QtCore/qjsonobject.h>
#include <QtCore/qlist.h>
#include <QtCore/qpair.h>
#include <QtCore/qvariant.h>
#include <QtCore/qstring.h>

namespace RestLink {
namespace Sql {

class ResourceInfo;
class Api;
class QueryOptions;

class SQL_EXPORT QueryBuilder
{
public:
    static bool canGenerate(const ResourceInfo &resource, const QueryOptions &options, Api *api);
    static QString selectStatement(const ResourceInfo &resource, const QueryOptions &options, Api *api);
    static QString insertStatement(const ResourceInfo &resource, const QVariantHash &data, Api *api);
    static QString updateStatement(const ResourceInfo &resource, const QVariantHash &data, const QueryOptions &options, Api *api);
    static QString deleteStatement(const ResourceInfo &resource, const QueryOptions &options, Api *api);

    static void extract(const ResourceInfo &resource, const QVariantHash &data, QStringList *columns, QStringList *values, Api *api);
    static QString whereClause(const QueryOptions &options, Api *api);

    static QString formatFieldName(const QString &name, Api *api);
    static QString formatTableName(const QString &name, Api *api);

    static QString formatValue(const QVariant &value, Api *api);
    static QString formatValue(const QVariant &value, const QMetaType &type, Api *api);

    static QStringList statementsFromScript(const QString &script);
};

class Expression : public QString
{
public:
    using QString::QString;
};

class QueryFilters
{
public:
    void andWhere(const QString &name, const QVariant &value)
    { m_filters.append({ .inclusive = true, .name = name, .op = "=", .value = value }); }
    void andWhere(const QString &name, const QString &op, const QVariant &value)
    { m_filters.append({ .inclusive = true, .name = name, .op = op, .value = value }); }
    void andWhere(const Expression &expr)
    { m_filters.append({ .expression = expr }); }

    void orWhere(const QString &name, const QVariant &value)
    { orWhere(name, "=", value); }
    void orWhere(const QString &name, const QString &op, const QVariant &value)
    { m_filters.append({ .inclusive = false, .name = name, .op = op, .value = value }); }
    void orWhere(const Expression &expr)
    { m_filters.append({ .inclusive = false, .expression = expr }); }


private:
    struct Filter {
        bool inclusive;
        QString name;
        QString op;
        QVariant value;
        QString expression;
    };
    QList<Filter> m_filters;

    friend class QueryBuilder;
};

class QueryOptions
{
public:
    QueryFilters filters;

    QString sortField;
    Qt::SortOrder sortOrder = Qt::DescendingOrder;

    int limit = 0;
    int offset = 0;

    bool withRelations = false;
};

} // namespace Sql
} // namespace RestLink

#endif // QUERYBUILDER_H
