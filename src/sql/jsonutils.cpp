#include "jsonutils.h"

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonvalue.h>

#include <QtSql/qsqlfield.h>

namespace RestLink {
namespace Sql {

QSqlRecord JsonUtils::recordFromRawData(const QByteArray &rawData, const QJsonObject &definition)
{
    return recordFromObject(QJsonDocument::fromJson(rawData).object(), definition);
}

QJsonObject JsonUtils::objectFromRawData(const QByteArray &rawData, const QJsonObject &definition)
{
    return QJsonDocument::fromJson(rawData).object();
}

QSqlRecord JsonUtils::recordFromObject(const QJsonObject &object, const QJsonObject &definition)
{
    QSqlRecord record;
    const QJsonArray fields = definition.value("fields").toArray();

    for (const QJsonValue &val : fields) {
        QJsonObject field = val.toObject();
        QString name = field.value("name").toString();

        if (!object.contains(name))
            continue;

        QString typeName = field.value("type").toString();
        QMetaType metaType = QMetaType::fromName(typeName.toUtf8());

        QVariant value = object.value(name).toVariant();
        if (metaType.isValid())
            value.convert(metaType);

        QSqlField sqlField(name, metaType);
        sqlField.setValue(value);
        record.append(sqlField);
    }

    return record;
}

QJsonObject JsonUtils::objectFromRecord(const QSqlRecord &record, const QJsonObject &definition)
{
    QJsonObject object;

    for (int i(0); i < record.count(); ++i)
        object.insert(record.fieldName(i), QJsonValue::fromVariant(record.value(i)));

    return object;
}

} // namespace Sql
} // namespace RestLink
