#include "sqlutils.h"

#include <QtCore/qjsonobject.h>

#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>

namespace RestLink {

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

QString SqlUtils::formatValue(const QVariant &value)
{
    return value.toString();
}

QStringList SqlUtils::formatValues(const QVariantList &values)
{
    QStringList strings(values.size());
    std::transform(values.begin(), values.end(), strings.begin(), &formatValue);
    return strings;
}

}
