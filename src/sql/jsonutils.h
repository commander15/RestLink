#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <QtCore/qjsonobject.h>

#include <QtSql/qsqlrecord.h>

namespace RestLink {
namespace Sql {

class ModelManager;

class JsonUtils
{
public:
    static QSqlRecord recordFromRawData(const QByteArray &rawData, const QJsonObject &definition);
    static QJsonObject objectFromRawData(const QByteArray &rawData, const QJsonObject &definition);

    static QSqlRecord recordFromObject(const QJsonObject &object, const QJsonObject &definition);
    static QJsonObject objectFromRecord(const QSqlRecord &record, const QJsonObject &definition = QJsonObject());
};

} // namespace Sql
} // namespace RestLink

#endif // JSONUTILS_H
