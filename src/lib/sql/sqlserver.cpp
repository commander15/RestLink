#include "sqlserver.h"
#include "sqlserver_p.h"

#include <RestLink/queryparameter.h>
#include <RestLink/serverresponse.h>
#include <RestLink/sqlutils.h>

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlerror.h>

namespace RestLink {

SqlServer::SqlServer(QObject *parent)
    : Server(new SqlServerPrivate(this), parent)
{
}

QStringList SqlServer::supportedSchemes() const
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

bool SqlServer::init()
{
    return true;
}

void SqlServer::cleanup()
{
    RESTLINK_D(SqlServer);
    for (const SqlServerPrivate::DatabaseInfos &info : d->databaseInfos)
        QSqlDatabase::removeDatabase(info.connection);
}

bool SqlServer::maintain()
{
    return true;
}

void SqlServer::handleGet(const Request &request, QSqlQuery *query, ServerResponse *response)
{
    RESTLINK_D(SqlServer);

    QString statement;
    bool single = false;

    QString table;
    QStringList filters;

    int page = 0;
    int limit = 0;

    if (request.endpoint() == "/query") {
        statement = request.header("X-SQL-QUERY").value().toString();
    } else {
        // Extracting path data
        QString primary;
        d->extractDataFromPath(request.urlPath(), &table, &primary);

        // Statement building
        statement = "SELECT * FROM " + table;

        if (!primary.isEmpty()) {
            statement.append(" WHERE id = " + primary);
            single = true;
        } else {
            // Extracting query data
            QString sortField;
            Qt::SortOrder sortOrder = Qt::AscendingOrder;
            d->extractDataFromQueries(request.queryParameters(), &filters, &sortField, &sortOrder, &page, &limit);

            if (!filters.isEmpty())
                statement.append(" WHERE " + filters.join(" AND "));

            if (!sortField.isEmpty())
                statement.append(QStringLiteral(" ORDER BY %1 %2").arg(sortField).arg(sortOrder == Qt::AscendingOrder ? "ASC" : "DESC"));

            if (page > 0) {
                if (limit <= 0)
                    limit = 50;

                const int offset = (page - 1) * limit;
                statement.append(QStringLiteral(" LIMIT %1 OFFSET %2").arg(limit).arg(offset));
            }
        }
    }

    query->setForwardOnly(true);
    if (query->exec(statement)) {
        QJsonObject object;

        if (single && query->next()) {
            object = SqlUtils::recordToJsonObject(query->record());
        } else if (!single && query->next()) {
            QJsonArray results;
            do {
                results.append(SqlUtils::recordToJsonObject(query->record()));

                if (query->size() > 0)
                    response->updateDownloadProgress(query->at(), query->size());
            } while (query->next());
            object.insert("results", results);
            query->finish();

            if (page > 0 && limit > 0) {
                object.insert("current_page", page);
                object.insert("items_per_page", limit);

                if (query->exec("SELECT COUNT(*) FROM " + table) && query->next()) {
                    const double count = query->value(0).toDouble();
                    object.insert("item_count", count);
                    object.insert("total_pages", qCeil(count / limit));
                }
            }
        }

        response->setBody(object);
        response->setHttpStatusCode(!object.isEmpty() ? 200 : 404);
    } else {
        handleError(query, response);
    }
}

void SqlServer::handlePost(const Request &request, const Body &body, QSqlQuery *query, ServerResponse *response)
{
    //
}

void SqlServer::handlePut(const Request &request, const Body &body, QSqlQuery *query, ServerResponse *response)
{
    //
}

void SqlServer::handlePatch(const Request &request, const Body &body, QSqlQuery *query, ServerResponse *response)
{
    //
}

void SqlServer::handleDelete(const Request &request, QSqlQuery *query, ServerResponse *response)
{
    //
}

void SqlServer::handleError(QSqlQuery *query, ServerResponse *response)
{
    handleError(query->lastError(), response);
}

void SqlServer::handleError(const QSqlError &error, ServerResponse *response)
{
    QJsonObject e;
    e.insert("code", error.nativeErrorCode());
    e.insert("message", error.databaseText());
    e.insert("description", error.driverText());
    e.insert("type", error.type());
    response->setBody(e);
}

void SqlServer::processRequest(ApiBase::Operation operation, const Request &request, const Body &body, Response *r)
{
    RESTLINK_D(SqlServer);

    ServerResponse *response = static_cast<ServerResponse *>(r);

    QSqlDatabase db = d->databaseFromUrl(r->property("dbUrl").toUrl());

    if (!db.isValid()) {
        //
        return;
    }

    if (!db.isOpen()) {
        handleError(db.lastError(), response);
        return;
    }

    QSqlQuery query(db);

    switch (operation) {
    case ApiBase::GetOperation:
        handleGet(request, &query, response);
        break;

    case ApiBase::PostOperation:
        handlePost(request, body, &query, response);
        break;

    case ApiBase::PutOperation:
        handlePut(request, body, &query, response);
        break;

    case ApiBase::PatchOperation:
        handlePatch(request, body, &query, response);
        break;

    case ApiBase::DeleteOperation:
        handleDelete(request, &query, response);
        break;

    default:
        break;
    }

    response->complete();
}

Response *SqlServer::createResponse(ApiBase::Operation operation, const Request &request, const Body &body, Api *api)
{
    Response *response = Server::createResponse(operation, request, body, api);
    response->setProperty("dbUrl", api->url());
    return response;
}

SqlServerPrivate::SqlServerPrivate(SqlServer *q)
    : ServerPrivate(Server::Synchronous, q)
{
}

int SqlServerPrivate::extractDataFromPath(const QString &path, QString *table, QString *primary)
{
    const QStringList paths = path.split('/', Qt::SkipEmptyParts);
    *table = (paths.size() > 0 ? paths.at(0) : QString());
    *primary = (paths.size() > 1 ? paths.at(1) : QString());
    return paths.size();
}

void SqlServerPrivate::extractDataFromQueries(const QList<QueryParameter> &queries, QStringList *filters, QString *sortField, Qt::SortOrder *sortOrder, int *page, int *limit)
{
    for (const QueryParameter &query : queries) {
        const QString name = query.name();
        if (name == "page") {
            *page = query.value().toInt();
        } else if (name == "limit") {
            *limit = query.value().toInt();
        } else if (name == "sort_by") {
            const QStringList expression = query.value().toString().split(",", Qt::SkipEmptyParts);
            if (expression.size() == 2) {
                *sortField = expression.at(0);
                *sortOrder = (expression.at(1) == "asc" ? Qt::AscendingOrder : Qt::DescendingOrder);
            }
        } else if (name.contains('.')) {
            const QStringList expression = name.split('.');
            const QString field = (expression.size() > 0 ? expression.at(0) : QString());
            const QString op = (expression.size() > 1 ? expression.at(1) : QString());

            if (!field.isEmpty() && !op.isEmpty()) {
                if (op == "gt")
                    filters->append(field + " > " + SqlUtils::formatValue(query.value()));
                else if (op == "gte")
                    filters->append(field + " >= " + SqlUtils::formatValue(query.value()));
                else if (op == "lt")
                    filters->append(field + " < " + SqlUtils::formatValue(query.value()));
                else if (op == "lte")
                    filters->append(field + " <= " + SqlUtils::formatValue(query.value()));
                else if (op == "eq")
                    filters->append(field + " = " + SqlUtils::formatValue(query.value()));
                else if (op == "neq")
                    filters->append(field + " != " + SqlUtils::formatValue(query.value()));
                else if (op == "like")
                    filters->append(field + " LIKE " + SqlUtils::formatValue(query.value()));
                else if (op == "in")
                    filters->append(field + " IN (" + SqlUtils::formatValues(query.values()).join(", ") + ")");
                else if (op == "notin")
                    filters->append(field + " NOT IN (" + SqlUtils::formatValues(query.values()).join(", ") + ")");
            }
        } else {
            filters->append(name + " = " + SqlUtils::formatValue(query.value()));
        }
    }
}

QSqlDatabase SqlServerPrivate::databaseFromUrl(const QUrl &url)
{
    if (databaseInfos.contains(url)) {
        DatabaseInfos &info = databaseInfos[url];
        return QSqlDatabase::database(info.connection);
    }

    static int index = 0;
    const QString connection = QStringLiteral("RestLink_%1").arg(index++);
    {
        QSqlDatabase db = QSqlDatabase::addDatabase('Q' + url.scheme().toUpper(), connection);
        db.setUserName(url.userName());
        db.setPassword(url.password());
        db.setHostName(url.host());
        db.setPort(url.port());
        db.setDatabaseName(url.path().mid(1));
    }

    DatabaseInfos info;
    info.connection = connection;
    //db.lastUsed = QDateTime::currentDateTime();
    databaseInfos.insert(url, info);

    return QSqlDatabase::database(info.connection);
}

} // namespace RestLink
