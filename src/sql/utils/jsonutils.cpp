#include "jsonutils.h"

#include <meta/resourceinfo.h>

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonvalue.h>

#include <QtSql/qsqlfield.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqldriver.h>

namespace RestLink {
namespace Sql {

QSqlRecord JsonUtils::recordFromRawData(const QByteArray &rawData, const ResourceInfo &resource)
{
    return recordFromObject(QJsonDocument::fromJson(rawData).object(), resource);
}

QJsonObject JsonUtils::objectFromRawData(const QByteArray &rawData)
{
    return QJsonDocument::fromJson(rawData).object();
}

QSqlRecord JsonUtils::recordFromObject(const QJsonObject &object, const ResourceInfo &resource)
{
    QSqlRecord record;
    const QStringList fields = resource.fieldNames();

    for (const QString &name : fields) {
        if (!object.contains(name))
            continue;

        QMetaType metaType = resource.fieldType(name);

        QVariant value = object.value(name).toVariant();
        if (metaType.isValid())
            value.convert(metaType);

        QSqlField sqlField(name, metaType);
        sqlField.setValue(value);
        record.append(sqlField);
    }

    return record;
}

QJsonObject JsonUtils::objectFromRecord(const QSqlRecord &record, const ResourceInfo &resource)
{
    QJsonObject object;

    for (int i(0); i < record.count(); ++i) {
        if (!record.isNull(i))
            object.insert(record.fieldName(i), QJsonValue::fromVariant(record.value(i)));
        else
            object.insert(record.fieldName(i), QJsonValue());
    }

    return object;
}

QJsonObject JsonUtils::objectFromQuery(const QSqlQuery &query)
{
    const QSqlDriver *driver = query.driver();

    QJsonObject object;
#ifdef RESTLINK_DEBUG
    object.insert("statement", query.isActive() ? query.executedQuery() : query.lastQuery());
#endif

    if (query.isSelect()) {
        if (driver->hasFeature(QSqlDriver::QuerySize)) {
            int size = query.size();
            object.insert("size", size < 0 ? 0 : size);
        }
    } else {
        int numRowsAffected = query.numRowsAffected();
        object.insert("num_rows_affected", numRowsAffected < 0 ? 0 : numRowsAffected);

        if (driver->hasFeature(QSqlDriver::LastInsertId))
            object.insert("last_insert_id", QJsonValue::fromVariant(query.lastInsertId()));
    }

    const QSqlError lastError = query.lastError();
    if (lastError.type() != QSqlError::NoError)
        object.insert("error", objectFromError(lastError));

    return object;
}

QJsonObject JsonUtils::objectFromError(const QSqlError &error)
{
    if (!error.isValid() || error.type() == QSqlError::NoError)
        return QJsonObject();

    QJsonObject object;
    object.insert("code", error.nativeErrorCode());
    object.insert("message", error.text());

    QString type;
    switch (error.type()) {
    case QSqlError::ConnectionError:  type = "connection error";  break;
    case QSqlError::StatementError:   type = "statement error";   break;
    case QSqlError::TransactionError: type = "transaction error"; break;
    default:                          type = "unknown error";     break;
    }

    object.insert("type", type);
    return object;
}

} // namespace Sql
} // namespace RestLink
