#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <global.h>

#include <QtCore/qjsonobject.h>

#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlerror.h>

class QSqlQuery;

namespace RestLink {
namespace Sql {

class ResourceInfo;

class SQL_EXPORT JsonUtils
{
public:
    static QSqlRecord recordFromRawData(const QByteArray &rawData, const ResourceInfo &resource);
    static QJsonObject objectFromRawData(const QByteArray &rawData);

    static QSqlRecord recordFromObject(const QJsonObject &object, const ResourceInfo &resource);
    static QJsonObject objectFromRecord(const QSqlRecord &record, const ResourceInfo &resource);

    static QJsonObject objectFromQuery(const QSqlQuery &query);
    static QJsonObject objectFromError(const QSqlError &error);
};

} // namespace Sql
} // namespace RestLink

#endif // JSONUTILS_H
