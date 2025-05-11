#ifndef QUERYBUILDER_H
#define QUERYBUILDER_H

#include <QtCore/qjsonobject.h>
#include <QtCore/qlist.h>
#include <QtCore/qpair.h>
#include <QtCore/qvariant.h>
#include <QtCore/qstring.h>

namespace RestLink {
namespace Sql {

class ModelManager;
class QueryOptions;

class QueryBuilder
{
public:
    static QString selectStatement(const QJsonObject &definition, const QueryOptions &options, ModelManager *manager);
    static QString insertStatement(const QJsonObject &definition, const QVariantHash &data, ModelManager *manager);
    static QString updateStatement(const QJsonObject &definition, const QVariantHash &data, const QueryOptions &options, ModelManager *manager);
    static QString deleteStatement(const QJsonObject &definition, const QueryOptions &options, ModelManager *manager);

    static void extract(const QJsonObject &definition, const QVariantHash &data, QStringList *columns, QStringList *values, ModelManager *manager);
    static QString whereClause(const QueryOptions &options, ModelManager *manager);
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
};

} // namespace Sql
} // namespace RestLink

#endif // QUERYBUILDER_H
