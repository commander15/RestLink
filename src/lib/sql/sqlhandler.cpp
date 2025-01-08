#include "sqlhandler.h"
#include "sqlhandler_p.h"

#include <RestLink/private/sqlresponse_p.h>
#include <RestLink/body.h>
#include <RestLink/private/sqlutils.h>

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlerror.h>

namespace RestLink {

SqlHandler::SqlHandler(QObject *parent)
    : ThreadedNetworkHandler(Synchronous, parent)
{
}

QStringList SqlHandler::schemes() const
{
    static QStringList schemes;
    if (schemes.isEmpty()) {
        schemes = QSqlDatabase::drivers();
        std::transform(schemes.begin(), schemes.end(), schemes.begin(), [](const QString &driver) {
            return driver.mid(1).toLower();
        });
    }
    return schemes;
}

Response *SqlHandler::sendRequest(ApiBase::Operation operation, const Request &request, const Body &body, Api *api)
{
    QJsonValue value;
    {
        QJsonDocument doc = QJsonDocument::fromJson(body.data());
        if (doc.isObject())
            value = doc.object();
        else if (doc.isArray())
            value = doc.array();
    }

    SqlResponse *response = new SqlResponse(api);
    response->setOperation(operation);
    response->setRequest(request);
    response->setNetworkRequest(createNetworkRequest(operation, request, body, api));
    response->setData(value);

    if (request.endpoint() == "/query" && request.hasHeader("X-SQL-QUERY")) {
        response->setQuery(request.header("X-SQL-QUERY").value().toString(), false);
    } else {
        const QStringList path = request.endpoint().split('/', Qt::SkipEmptyParts);
        const QString &tableName = (path.size() > 0 ? path.at(0) : QString());
        const QString primary = (path.size() > 1 ? path.at(1) : QString());

        bool single = false;
        const QString query = generateQuery(operation, request, value, &single);
        response->setQuery(query, single);
    }

    return response;
}

void SqlHandler::process(Response *re)
{
    SqlResponse *response = static_cast<SqlResponse *>(re);

    QSqlDatabase db = QSqlDatabase::database(dbConnectionForUrl(response->api()->url()));
    QSqlQuery query(db);
    query.prepare(response->query());
    query.setForwardOnly(true);

    QJsonObject object;

    if (query.exec()) {
        processQuery(&query, &object, response);
    } else {
        const QSqlError error = query.lastError();

        QJsonObject e;
        e.insert("code", error.nativeErrorCode());
        e.insert("message", error.databaseText());
        e.insert("description", error.driverText());
        e.insert("type", error.type());
        object.insert("error", e);
    }

    response->setData(object);
    response->complete();
}

void SqlHandler::processQuery(QSqlQuery *query, QJsonObject *object, SqlResponse *response)
{
    switch (response->operation()) {
    case Api::GetOperation:
        handleGetOperation(query, object, response);
        break;
    }
}

void SqlHandler::handleGetOperation(QSqlQuery *query, QJsonObject *object, SqlResponse *response)
{
    bool found = false;
    if (response->isSingleResult() && query->next()) {
        *object = SqlUtils::recordToJsonObject(query->record());
        found = true;
    } else if (!response->isSingleResult()) {
        QJsonArray array;
        const bool sizeAvailable = (query->size() > 0);
        while (query->next()) {
            array.append(SqlUtils::recordToJsonObject(query->record()));
            if (sizeAvailable)
                response->updateDownloadProgress(query->at() + 1, query->size());
            found = true;
        }
        object->insert("results", array);
    }

    response->setHttpStatusCode(found ? 200 : 404);
}

bool SqlHandler::init()
{
    return true;
}

void SqlHandler::cleanup()
{
    const QStringList connections = m_dbUrl.values();
    for (const QString &connection : connections)
        QSqlDatabase::removeDatabase(connection);
}

QString SqlHandler::generateQuery(ApiBase::Operation operation, const Request &request, const QJsonValue &body, bool *single) const
{
    const QStringList path = request.endpoint().split("/", Qt::SkipEmptyParts);
    const QString tableName = (path.size() > 0 ? path.at(0) : QString());
    const QString primaryValue = (path.size() > 1 ? path.at(1) : QString());

    QString query;

    if (operation == Api::GetOperation) {
        query = "SELECT * FROM " + tableName;
        if (!primaryValue.isEmpty())
            query.append(" WHERE id = " + primaryValue);
    }

    return query;
}

QString SqlHandler::dbConnectionForUrl(const QUrl &url)
{
    if (m_dbUrl.contains(url))
        return m_dbUrl.value(url);

    static int index = 0;

    const QString connection = QStringLiteral("RestLink_%1").arg(index++);

    QSqlDatabase db = QSqlDatabase::addDatabase('Q' + url.scheme().toUpper(), connection);
    db.setUserName(url.userName());
    db.setPassword(url.password());
    db.setHostName(url.host());
    db.setPort(url.port());
    db.setDatabaseName(url.path().mid(1));

    m_dbUrl.insert(url, connection);
    return connection;
}

} // namespace RestLink
