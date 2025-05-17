#ifndef RESTLINK_SQLUTILS_H
#define RESTLINK_SQLUTILS_H

#include <global.h>

#include <RestLink/global.h>

class QJsonObject;

class QSqlRecord;
class QSqlDatabase;

namespace RestLink {

class SQL_EXPORT SqlUtils
{
public:
    static QSqlRecord jsonToRecord(const QByteArray &json);

    static QJsonObject recordToJsonObject(const QSqlRecord &record);
    static QSqlRecord jsonObjectToRecord(const QJsonObject &object);

    static QString fieldName(const QString &name, const QSqlDatabase *database);
    static QString tableName(const QString &name, const QSqlDatabase *database);

    static QString formatValue(const QVariant &value, const QSqlDatabase *database);
    static QStringList formatValues(const QVariantList &values, const QSqlDatabase *database);
};

}

#endif // RESTLINK_SQLUTILS_H
