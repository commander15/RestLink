#include "queryrunner.h"

#include <debug.h>
#include <api.h>
#include <meta/resourceinfo.h>
#include <utils/jsonutils.h>

#include <QtCore/qjsonarray.h>

#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlrecord.h>

namespace RestLink {
namespace Sql {

QJsonObject QueryRunner::exec(const Query &query, Api *api, bool *success)
{
    bool succeeded = false;
    QSqlQuery sqlQuery = exec(query.statement, api, &succeeded);
    if (!succeeded) {
        if (success) *success = false;
        return JsonUtils::objectFromQuery(sqlQuery);
    } else {
        if (success) *success = true;
    }

    QJsonObject body = JsonUtils::objectFromQuery(sqlQuery);
    if (query.array) {
        QJsonArray data;
        while (sqlQuery.next())
            data.append(JsonUtils::objectFromRecord(sqlQuery.record(), ResourceInfo()));
        body.insert("data", data);
    } else if (sqlQuery.next()) {
        body.insert("data", JsonUtils::objectFromRecord(sqlQuery.record(), ResourceInfo()));
    } else {
        body.insert("data", QJsonValue());
    }

    return body;
}

QSqlQuery QueryRunner::exec(const QString &statement, Api *api, bool *success)
{
    if (statement.isEmpty()) {
        sqlWarning() << "Empty query detected !";
        return QSqlQuery();
    }

    sqlInfo() << statement;

    QSqlQuery sqlQuery(api->database());
    sqlQuery.setForwardOnly(true);
    if (!sqlQuery.exec(statement)) {
#ifdef RESTLINK_DEBUG
        const QString error = sqlQuery.lastError().databaseText();
        sqlWarning() << error;
#endif
        if (success) *success = false;
    } else {
        if (success) *success = true;
    }
    return sqlQuery;
}


} // namespace Sql
} // namespace RestLink
