#ifndef RESTLINK_SQLUTILS_H
#define RESTLINK_SQLUTILS_H

#include <RestLink/global.h>

class QJsonObject;

class QSqlRecord;

namespace RestLink {

class RESTLINK_EXPORT SqlUtils
{
public:
    static QJsonObject recordToJsonObject(const QSqlRecord &record);
    static QSqlRecord jsonObjectToRecord(const QJsonObject &object);
};

}

#endif // RESTLINK_SQLUTILS_H
