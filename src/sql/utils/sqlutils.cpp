#include "sqlutils.h"

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>

#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqldriver.h>

namespace RestLink {

QSqlRecord SqlUtils::jsonToRecord(const QByteArray &json)
{
    QJsonDocument doc = QJsonDocument::fromJson(json);
    return jsonObjectToRecord(doc.object());
}

QJsonObject SqlUtils::recordToJsonObject(const QSqlRecord &record)
{
    QJsonObject jsonObject;
    for (int i = 0; i < record.count(); ++i) {
        const QString fieldName = record.fieldName(i);
        const QVariant value = record.value(i);

        if (!value.isNull()) {
            jsonObject.insert(fieldName, QJsonValue::fromVariant(value));
        } else {
            jsonObject.insert(fieldName, QJsonValue());
        }
    }
    return jsonObject;
}

QSqlRecord SqlUtils::jsonObjectToRecord(const QJsonObject &object)
{
    QSqlRecord record;
    for (auto it = object.begin(); it != object.end(); ++it) {
        const QVariant value = it.value().toVariant();
        QSqlField field(it.key(), value.metaType());
        field.setValue(value);
        record.append(field);
    }
    return record;
}

QString SqlUtils::fieldName(const QString &name, const QSqlDatabase *database)
{
    return database->driver()->escapeIdentifier(name, QSqlDriver::FieldName);
}

QString SqlUtils::tableName(const QString &name, const QSqlDatabase *database)
{
    return database->driver()->escapeIdentifier(name, QSqlDriver::TableName);
}

QString SqlUtils::formatValue(const QVariant &value, const QSqlDatabase *database)
{
    QSqlField field(QString(), value.metaType());
    field.setValue(value);
    return database->driver()->formatValue(field);
}

QStringList SqlUtils::formatValues(const QVariantList &values, const QSqlDatabase *database)
{
    QStringList strings(values.size());
    std::transform(values.begin(), values.end(), strings.begin(), [database](const QVariant &value) {
        return formatValue(value, database);
    });
    return strings;
}

}
