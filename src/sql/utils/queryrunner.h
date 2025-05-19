#ifndef QUERYRUNNER_H
#define QUERYRUNNER_H

#include <global.h>

#include <QtCore/qjsonobject.h>

#include <QtSql/qsqlquery.h>

namespace RestLink {
namespace Sql {

class Api;

class SQL_EXPORT Query
{
public:
    QString statement;
    bool array = true;
};

class SQL_EXPORT QueryRunner
{
public:
    static QJsonObject exec(const Query &query, Api *api, bool *success = nullptr);
    static QSqlQuery exec(const QString &statement, Api *api, bool *success = nullptr);
};

} // namespace Sql
} // namespace RestLink

#endif // QUERYRUNNER_H
