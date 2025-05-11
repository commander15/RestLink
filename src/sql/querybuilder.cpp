#include "querybuilder.h"

#include "modelmanager.h"

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonvalue.h>

#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlfield.h>

namespace RestLink {
namespace Sql {

QString QueryBuilder::selectStatement(const QJsonObject &definition, const QueryOptions &options, ModelManager *manager)
{
    if (!manager)
        return QString();

    const QString table = definition.value("table").toString();
    if (table.isEmpty())
        return QString();

    QString statement = QStringLiteral("SELECT * FROM %1").arg(table);

    const QString whereClause = QueryBuilder::whereClause(options, manager);
    if (!whereClause.isEmpty())
        statement += ' ' + whereClause;

    if (!options.sortField.isEmpty()) {
        statement += QStringLiteral(" ORDER BY %1 %2")
                         .arg(options.sortField,
                              options.sortOrder == Qt::AscendingOrder ? "ASC" : "DESC");
    }

    if (options.limit > 0)
        statement += QStringLiteral(" LIMIT %1 OFFSET %2").arg(options.limit).arg(options.offset);

    return statement;
}

QString QueryBuilder::insertStatement(const QJsonObject &definition, const QVariantHash &data, ModelManager *manager)
{
    if (!manager)
        return QString();

    const QString table = definition.value("table").toString();
    if (table.isEmpty())
        return QString();

    QStringList columns;
    QStringList values;
    extract(definition, data, &columns, &values, manager);

    if (columns.isEmpty())
        return QString();

    return QStringLiteral("INSERT INTO %1 (%2) VALUES (%3)")
        .arg(table, columns.join(", "), values.join(", "));
}

QString QueryBuilder::updateStatement(const QJsonObject &definition, const QVariantHash &data, const QueryOptions &options, ModelManager *manager)
{
    if (!manager)
        return QString();

    const QString table = definition.value("table").toString();
    if (table.isEmpty())
        return QString();

    QStringList columns;
    QStringList values;
    extract(definition, data, &columns, &values, manager);

    if (columns.isEmpty())
        return QString();

    QStringList setClauses;
    for (int i = 0; i < columns.size(); ++i)
        setClauses.append(QString("%1 = %2").arg(columns.at(i), values.at(i)));
    QString setClause = setClauses.join(", ");

    QString whereClause = QueryBuilder::whereClause(options, manager);

    return QStringLiteral("UPDATE %1 SET %2%3")
        .arg(table, setClause, !whereClause.isEmpty() ? ' ' + whereClause : QString());
}

QString QueryBuilder::deleteStatement(const QJsonObject &definition, const QueryOptions &options, ModelManager *manager)
{
    if (!manager)
        return QString();

    const QString table = definition.value("table").toString();
    if (table.isEmpty())
        return QString();

    // Build the WHERE clause using the filters
    QString whereClause = QueryBuilder::whereClause(options, manager);

    return QStringLiteral("DELETE FROM %1%2")
        .arg(table, !whereClause.isEmpty() ? ' ' + whereClause : QString());
}

void QueryBuilder::extract(const QJsonObject &definition, const QVariantHash &data, QStringList *columns, QStringList *values, ModelManager *manager)
{
    if (!manager)
        return;

    // Retrieve fields from the definition
    QJsonArray fields = definition.value("fields").toArray();

    for (int i = 0; i < fields.size(); ++i) {
        QJsonObject field = fields.at(i).toObject();
        QString columnName = field.value("name").toString();

        // Only include the field if it's present in the data
        if (data.contains(columnName)) {
            if (columns) columns->append(columnName);

            // Get the QSqlField type from the definition
            QMetaType fieldType = QMetaType::fromName(field.value("type").toString().toUtf8());

            // Format value based on the field type
            QSqlField sqlField(columnName, fieldType);
            sqlField.setValue(data.value(columnName));
            values->append(manager->database().driver()->formatValue(sqlField));
        }
    }
}

QString QueryBuilder::whereClause(const QueryOptions &options, ModelManager *manager)
{
    if (!manager)
        return QString();

    QSqlDriver *driver = manager->database().driver();

    QStringList filters;

    for (const QueryFilters::Filter &filter : options.filters.m_filters) {
        QString expression;
        if (!filter.expression.isEmpty())
            expression = filter.expression;
        else {
            QSqlField field(filter.name, filter.value.metaType());
            field.setValue(filter.value);
            expression = QStringLiteral("%1 %3 %2")
                .arg(filter.name, driver->formatValue(field), filter.op);
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

} // namespace Sql
} // namespace RestLink
