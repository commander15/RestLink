#include "sqlmodel.h"

#include <QtCore/qjsonarray.h>

#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>

#include "sqlutils.h"

namespace RestLink {

SqlModel::SqlModel(const QString &table, SqlDatabase *database)
    : m_table(table)
    , m_configuration(database->configurationFor(table))
    , m_query(*database)
    , m_db(database)
{
}

QVariant SqlModel::primary() const
{
    const QString primary = m_configuration.value("primary").toString();
    return m_data.value(primary).toVariant();
}

void SqlModel::setPrimary(const QVariant &id)
{
    const QString primary = m_configuration.value("primary").toString();
    m_data.insert(primary, QJsonValue::fromVariant(id));
}

QVariant SqlModel::value(const QString &field)
{
    return m_data.value(field).toVariant();
}

void SqlModel::setValue(const QString &field, const QVariant &value)
{
    m_data.insert(field, QJsonValue::fromVariant(value));
}

QJsonObject SqlModel::data(bool all) const
{
    if (all)
        return m_data;

    QJsonObject data;

    // Handling fillable
    const QJsonArray fillable = m_configuration.value("fillable").toArray();
    const QStringList fields = m_data.keys();

    for (const QString &field : fields)
        if (fillable.contains(field))
            data.insert(field, m_data.value(field));

    return data;
}

void SqlModel::fill(const QJsonObject &object)
{
    // Handling fillable
    const QJsonArray fillable = m_configuration.value("fillable").toArray();
    const QStringList fields = object.keys();
    m_data = QJsonObject();
    for (const QString &field : fields)
        if (fillable.contains(field))
            m_data.insert(field, object.value(field));

    // Handling relations
    const QJsonArray relations = m_configuration.value("relations").toArray();
    m_relations.clear();
    for (const QJsonValue &relationValue : relations) {
        const QJsonObject relation = relationValue.toObject();
        const QString name = relation.value("name").toString();
        const QString foreignTable = relation.value("table").toString();
        const QString type = relation.value("type").toString().remove(' ');
        const QJsonValue value = object.value(name);

        if (value.isObject() && type.endsWith('1')) {
            SqlModel model(foreignTable, m_db);
            model.fill(value.toObject());
            m_relations.append(Relation(name, { model }));
        } else if (value.isArray() && type.endsWith('*')) {
            Relation relation;
            relation.first = name;

            const QJsonArray array = value.toArray();
            for (const QJsonValue &value : array) {
                SqlModel model(foreignTable, m_db);
                model.fill(value.toObject());
                relation.second.append(model);
            }

            m_relations.append(relation);
        }
    }
}

void SqlModel::fill(const QSqlRecord &record)
{
    // Handling fillable
    const QJsonArray fillable = m_configuration.value("fillable").toArray();
    m_data = QJsonObject();
    for (int i(0); i < record.count(); ++i) {
        const QString fieldName = record.fieldName(i);
        if (fillable.contains(fieldName))
            m_data.insert(fieldName, QJsonValue::fromVariant(record.value(fieldName)));
    }

    // Handling relations
    m_relations.clear();
}

bool SqlModel::get()
{
    const QString tableName = m_configuration.value("table").toString();

    QStringList statements;
    statements.append("SELECT * FROM " + SqlUtils::tableName(tableName, m_db));
    statements.append("WHERE " + SqlUtils::fieldName(tableName, m_db) + " = " + SqlUtils::formatValue(primary(), m_db));
    statements.append("LIMIT 1");

    if (m_query.exec(statements.join(' ')) && m_query.next()) {
        fill(m_query.record());
        m_query.finish();

        // For relations fetching
        QSqlQuery query(*m_db);

        // Handling relations
        const QJsonArray relations = m_configuration.value("relations").toArray();
        m_relations.clear();
        for (const QJsonValue &relationValue : relations) {
            const QJsonObject relation = relationValue.toObject();
            const QString name = relation.value("name").toString();
            const QString foreignTable = relation.value("table").toString();
            const QString foreignField = relation.value("foreign").toString();
            const QString type = relation.value("type").toString().remove(' ');

            if (type == "1->1" || type == "*-1") {
                // FK is in current table
                QVariant fkValue = m_data.value(foreignField).toVariant();
                if (!fkValue.isNull()) {
                    Relation rel;
                    rel.first = name;
                    rel.second.append(find(fkValue, foreignTable, m_db));
                    m_relations.append(rel);
                }
            } else if (type == "1<-1" || type == "1-*") {
                // FK is in related table
                Query q;
                q.filters.append(SqlUtils::fieldName(foreignField, m_db) + " = " + SqlUtils::formatValue(primary(), m_db));

                Relation rel;
                rel.first = name;
                rel.second = find(q, foreignTable, m_db);
                m_relations.append(rel);
            } else if (type == "*-*") {
                Relation rel;
                rel.first = name;

                Query q;
                q.filters.append(SqlUtils::fieldName(foreignField, m_db) + " = " + SqlUtils::formatValue(primary(), m_db));

                const QString pivotTable = relation.value("pivot").toString();
                const QString primaryField = m_configuration.value("primary").toString();
                // Handling * - * (pivot table usage required)

                QStringList statements;
                statements.append("SELECT " + SqlUtils::fieldName(foreignField, m_db) + " FROM " + SqlUtils::tableName(pivotTable, m_db));
                statements.append("WHERE " + SqlUtils::fieldName(primaryField, m_db) + " = " + SqlUtils::formatValue(primary(), m_db));
                if (query.exec(statements.join(' '))) {
                    while (query.next()) {
                        const QVariant primary = query.value(0);
                        rel.second.append(find(primary, foreignTable, m_db));
                    }
                }
            }
        }

        return true;
    } else {
        return false;
    }
}

bool SqlModel::get(const QVariant &id)
{
    setPrimary(id);
    return get();
}

bool SqlModel::save()
{
    if (!primary().isValid())
        return insert();
    else
        return update();
}

} // namespace RestLink
