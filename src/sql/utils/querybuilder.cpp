#include "querybuilder.h"

#include <api.h>
#include <meta/resourceinfo.h>

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonvalue.h>

#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlfield.h>

namespace RestLink {
namespace Sql {

bool QueryBuilder::canGenerate(const ResourceInfo &resource, const QueryOptions &options, Api *api)
{
    return resource.isValid() && api;
}

QString QueryBuilder::selectStatement(const ResourceInfo &resource, const QueryOptions &options, Api *api)
{
    if (!canGenerate(resource, options, api))
        return QString();

    QString statement = QStringLiteral("SELECT * FROM %1").arg(formatTableName(resource.table(), api));

    const QString whereClause = QueryBuilder::whereClause(options, api);
    if (!whereClause.isEmpty())
        statement.append(' ' + whereClause);

    if (!options.sortField.isEmpty()) {
        statement += QStringLiteral(" ORDER BY %1 %2")
                         .arg(options.sortField,
                              options.sortOrder == Qt::AscendingOrder ? "ASC" : "DESC");
    }

    if (options.limit > 0)
        statement.append(QStringLiteral(" LIMIT %1").arg(options.limit));

    if (options.offset > 0)
        statement.append(QStringLiteral("OFFSET %1").arg(options.offset));

    return statement;
}

QString QueryBuilder::insertStatement(const ResourceInfo &resource, const QVariantHash &data, Api *api)
{
    if (!canGenerate(resource, QueryOptions(), api))
        return QString();

    const QString table = resource.table();
    if (table.isEmpty())
        return QString();

    QStringList columns;
    QStringList values;
    extract(resource, data, &columns, &values, api);

    if (columns.isEmpty())
        return QString();

    return QStringLiteral("INSERT INTO %1 (%2) VALUES (%3)")
        .arg(formatTableName(table, api), columns.join(", "), values.join(", "));
}

QString QueryBuilder::updateStatement(const ResourceInfo &resource, const QVariantHash &data, const QueryOptions &options, Api *api)
{
    if (!canGenerate(resource, options, api))
        return QString();

    const QString table = resource.table();
    if (table.isEmpty())
        return QString();

    QStringList columns;
    QStringList values;
    extract(resource, data, &columns, &values, api);

    if (columns.isEmpty())
        return QString();

    QStringList setClauses;
    for (int i = 0; i < columns.size(); ++i)
        setClauses.append(QString("%1 = %2").arg(columns.at(i), values.at(i)));
    QString setClause = setClauses.join(", ");

    QString whereClause = QueryBuilder::whereClause(options, api);

    return QStringLiteral("UPDATE %1 SET %2%3")
        .arg(formatTableName(table, api), setClause, !whereClause.isEmpty() ? ' ' + whereClause : QString());
}

QString QueryBuilder::deleteStatement(const ResourceInfo &resource, const QueryOptions &options, Api *api)
{
    if (!canGenerate(resource, options, api))
        return QString();

    const QString table = resource.table();
    if (table.isEmpty())
        return QString();

    // Build the WHERE clause using the filters
    QString whereClause = QueryBuilder::whereClause(options, api);

    return QStringLiteral("DELETE FROM %1%2")
        .arg(formatTableName(table, api), !whereClause.isEmpty() ? ' ' + whereClause : QString());
}

void QueryBuilder::extract(const ResourceInfo &resource, const QVariantHash &data, QStringList *columns, QStringList *values, Api *api)
{
    if (!canGenerate(resource, QueryOptions(), api))
        return;

    QStringList fieldNames = resource.fieldNames();
    fieldNames.removeOne(resource.primaryKey());
    for (const QString &fieldName : std::as_const(fieldNames)) {
        if (!data.contains(fieldName))
            continue;

        if (columns)
            columns->append(formatFieldName(fieldName, api));

        if (values)
            values->append(formatValue(data.value(fieldName), resource.fieldType(fieldName), api));
    }
}

QString QueryBuilder::whereClause(const QueryOptions &options, Api *api)
{
    if (!api)
        return QString();

    QStringList filters;

    for (const QueryFilters::Filter &filter : options.filters.m_filters) {
        QString expression;
        if (!filter.expression.isEmpty())
            expression = filter.expression;
        else {
            expression = QStringLiteral("%1 %3 %2")
                .arg(formatFieldName(filter.name, api), formatValue(filter.value, filter.value.metaType(), api), filter.op);
        }

        if (expression.isEmpty())
            continue;

        if (filters.isEmpty())
            filters.append(expression);
        else
            filters.append(filter.inclusive ? "AND " : "OR " + expression);
    }

    if (filters.isEmpty())
        return QString();

    return QStringLiteral("WHERE ") + filters.join(' ');
}

QString QueryBuilder::formatFieldName(const QString &name, Api *api)
{
    if (name.contains('.')) {
        const QStringList parts = name.split('.', Qt::SkipEmptyParts);
        if (parts.size() == 2)
            return formatTableName(parts.first(), api) + '.' + formatFieldName(parts.last(), api);
        else
            return name;
    }

    return api->database().driver()->escapeIdentifier(name, QSqlDriver::FieldName);
}

QString QueryBuilder::formatTableName(const QString &name, Api *api)
{
    return api->database().driver()->escapeIdentifier(name, QSqlDriver::TableName);
}

QString QueryBuilder::formatValue(const QVariant &value, Api *api)
{
    return formatValue(value, value.metaType(), api);
}

QString QueryBuilder::formatValue(const QVariant &value, const QMetaType &type, Api *api)
{
    QSqlField field(QStringLiteral("x"), type);
    field.setValue(value);
    return api->database().driver()->formatValue(field);
}

QStringList QueryBuilder::statementsFromScript(const QString &script)
{
    QStringList statements;
    QStringList input = script.trimmed().split('\n');

    QString statement;
    QString delimiter = ";";
    for (QString &line : input) {
        line = line.trimmed();

        // We skip empty lines
        if (line.isEmpty())
            continue;

        // We skip comments
        if (line.startsWith("--"))
            continue;

        // We change delimiter
        if (line.contains("DELIMITER", Qt::CaseInsensitive)) {
            line.remove("DELIMITER", Qt::CaseInsensitive);
            delimiter = line.trimmed();
        }

        statement.append(line);

        if (line.endsWith(delimiter)) {
            statements.append(statement);
            statement.clear();
        }
    }

    // If there is only one statement without delimiter
    if (statements.isEmpty() && !statement.isEmpty())
        statements.append(statement);

    return statements;
}

} // namespace Sql
} // namespace RestLink
