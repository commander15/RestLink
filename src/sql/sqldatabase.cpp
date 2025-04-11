#include "sqldatabase.h"

#include <RestLink/serverrequest.h>
#include <RestLink/serverresponse.h>

#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlindex.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>
#include <QtSql/qsqlerror.h>
#include <QtSql/qsqldriver.h>

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonvalue.h>
#include <QtCore/qvariant.h>

#include "sqlutils.h"

namespace RestLink {

SqlDatabase::SqlDatabase(const QUrl &url)
    : QSqlDatabase('Q' + url.scheme().toUpper())
    , m_url(url)
{
    setHostName(url.host());
    setPort(url.port());
    setUserName(url.userName());
    setPassword(url.password());
    setDatabaseName(url.path());
}

QString SqlDatabase::endpoint() const
{
    return m_url.path();
}

QUrl SqlDatabase::url() const
{
    return m_url;
}

void SqlDatabase::updateConfiguration(const QJsonObject &config)
{
    m_configuration = config;
}

bool SqlDatabase::canProcessRequest(const ServerRequest &request) const
{
    return request.baseUrl() == m_url;
}

void SqlDatabase::processRequest(ApiBase::Operation operation, const ServerRequest &request, ServerResponse *response)
{
    const QString endpoint = request.endpoint();

    if (endpoint == "/query") {
        query(request, response);
        return;
    }

    if (endpoint == "/tables") {
        tables(request, response);
        return;
    }

    AbstractResourceController::processRequest(operation, request, response);
}

void SqlDatabase::query(const ServerRequest &request, ServerResponse *response)
{
    QSqlQuery query = exec(request.body().data(), response);
    if (!query.isActive())
        return;

    QJsonArray body;
    while (query.next())
        body.append(SqlUtils::recordToJsonObject(query.record()));

    response->setHttpStatusCode(200);
    response->setBody(body);
    response->complete();
}

void SqlDatabase::tables(const ServerRequest &request, ServerResponse *response)
{
    QJsonArray body;

    const QStringList tableNames = QSqlDatabase::tables();
    for (const QString &tableName : tableNames) {
        QJsonObject table;
        table.insert("name", tableName);

        QJsonArray fields;
        const QSqlRecord record = QSqlDatabase::record(tableName);
        for (int i(0); i < record.count(); ++i) {
            QJsonObject field;

            const QSqlField sqlField = record.field(i);
            field.insert("name", sqlField.name());
            field.insert("type", sqlField.metaType().name());

            if (sqlField.length() > 0)
                field.insert("length", sqlField.length());

            const QVariant defaultValue = sqlField.defaultValue();
            if (defaultValue.isValid())
                field.insert("default", QJsonValue::fromVariant(defaultValue));

            field.insert("required", sqlField.isReadOnly());

            field.insert("auto", sqlField.isAutoValue());

            fields.append(field);
        }

        table.insert("fields", fields);

        body.append(table);
    }

    response->setHttpStatusCode(200);
    response->setBody(body);
    response->complete();
}

void SqlDatabase::index(const ServerRequest &request, ServerResponse *response)
{
    if (!canProcess(request, response))
        return;

    QJsonObject body;

    int currentPage = 1;
    int lastPage = 1;
    int perPage = 50;
    int total = 0;
    {
        QString statement = this->selectStatement(request);

        const QString whereClause = this->whereClause(request);
        if (!whereClause.isEmpty())
            statement.append(' ' + whereClause);

        const QString orderByClause = this->orderByClause(request);
        if (!orderByClause.isEmpty())
            statement.append(' ' + orderByClause);

        const QString limitClause = this->limitOffsetClause(request, &currentPage, &perPage);
        if (!limitClause.isEmpty())
            statement.append(' ' + limitClause);

        QSqlQuery query = exec(statement, response);
        if (!query.isActive()) {
            return;
        }

        QJsonArray result;
        while (query.next())
            result.append(SqlUtils::recordToJsonObject(query.record()));
        body.insert("data", result);

        statement = countStatement(request);
        if (!whereClause.isEmpty())
            statement.append(' ' + whereClause);

        query = exec(statement);
        if (!query.isActive()) {
            return;
        }

        query.next();
        total = query.value(0).toInt();

        if (total < perPage)
            lastPage = currentPage;
        else
            lastPage = qCeil(double(total) / perPage);
    }

    body.insert("current_page", currentPage);
    body.insert("last_page", lastPage);
    body.insert("per_page", perPage);
    body.insert("total", total);

    response->setHttpStatusCode(200);
    response->setBody(body);
    response->complete();
}

void SqlDatabase::show(const ServerRequest &request, ServerResponse *response)
{
    if (!canProcess(request, response))
        return;

    QString statement = selectStatement(request);

    const QString whereClause = this->whereClause(request, true);
    if (!whereClause.isEmpty())
        statement.append(' ' + whereClause);

    statement.append(" LIMIT 1");

    QSqlQuery query = exec(statement, response);
    if (!query.isActive())
        return;

    if (query.next()) {
        response->setHttpStatusCode(200);
        response->setBody(SqlUtils::recordToJsonObject(query.record()));
    } else {
        response->setHttpStatusCode(404);
    }

    response->complete();
}

void SqlDatabase::update(const ServerRequest &request, ServerResponse *response)
{
    if (!canProcess(request, response))
        return;
}

void SqlDatabase::store(const ServerRequest &request, ServerResponse *response)
{
    if (!canProcess(request, response))
        return;
}

void SqlDatabase::destroy(const ServerRequest &request, ServerResponse *response)
{
    if (!canProcess(request, response))
        return;

    QString statement = deleteStatement(request);

    const QString whereClause = this->whereClause(request, true);
    if (!whereClause.isEmpty())
        statement.append(' ' + whereClause);

    QSqlQuery query = exec(statement, response);
    if (!query.isActive())
        return;

    if (query.numRowsAffected() > 0)
        response->setHttpStatusCode(200);
    else
        response->setHttpStatusCode(500);
    response->complete();
}

QString SqlDatabase::tableName(const ServerRequest &request) const
{
    const QString resource = request.resource().replace('.', '_');
    const QStringList tableNames = QSqlDatabase::tables();

    auto it = std::find_if(tableNames.begin(), tableNames.end(), [&resource](const QString &tableName) {
        return resource == tableName.toLower();
    });

    return (it != tableNames.end() ? *it : resource);
}

QSqlRecord SqlDatabase::tableRecord(const ServerRequest &request, bool filled) const
{
    if (!filled)
        return QSqlDatabase::record(tableName(request));

    QSqlRecord record = QSqlDatabase::record(tableName(request));
    const QSqlRecord record2 = SqlUtils::jsonToRecord(request.body().data());

    for (int i(0); i < record2.count(); ++i)
        record.setValue(record2.fieldName(i), record2.value(i));

    return record;
}

QJsonObject SqlDatabase::configurationFor(const QString &tableName) const
{
    const QJsonArray resources = m_configuration.value("resources").toArray();
    auto it = std::find_if(resources.begin(), resources.end(), [&tableName](const QJsonValue &value) {
        return value.toObject().value("table") == tableName;
    });

    QJsonObject configuration = (it != resources.end() ? it->toObject() : QJsonObject());

    if (!configuration.contains("primary")) {
        const QSqlIndex index = primaryIndex(tableName);
        configuration.insert("primary", index.count() > 0 ? index.fieldName(0) : "id");
    }

    if (!configuration.contains("fillable")) {
        const QString primary = configuration.value("primary").toString();
        QJsonArray fillable;

        const QSqlRecord record = QSqlDatabase::record(tableName);
        for (int i(0); i < record.count(); ++i) {
            const QString fieldName = record.fieldName(i);

            if (fieldName == primary)
                continue;

            fillable.append(fieldName);
        }
    }

    return configuration;
}

QSqlQuery SqlDatabase::exec(const QString &query, ServerResponse *response)
{
    if (response) {
        QList<Header> headers;
        headers.append(Header("X-SQL-QUERY", query));
        response->setHeaders(headers);
    }

    QSqlQuery sqlQuery(*this);
    sqlQuery.setForwardOnly(true);

    if (!sqlQuery.exec(query) && response) {
        handleError(sqlQuery.lastError(), response);
        return QSqlQuery();
    }

    return sqlQuery;
}

bool SqlDatabase::canProcess(const ServerRequest &request, ServerResponse *response) const
{
    return !request.resource().isEmpty();
}

QString SqlDatabase::selectStatement(const ServerRequest &request) const
{
    return "SELECT * FROM " + driver()->escapeIdentifier(tableName(request), QSqlDriver::TableName);
}

QString SqlDatabase::countStatement(const ServerRequest &request) const
{
    return "SELECT COUNT(*) FROM " + driver()->escapeIdentifier(tableName(request), QSqlDriver::TableName);
}

QString SqlDatabase::deleteStatement(const ServerRequest &request) const
{
    return "DELETE FROM " + driver()->escapeIdentifier(tableName(request), QSqlDriver::TableName);
}

QString SqlDatabase::whereClause(const ServerRequest &request, bool idOnly) const
{
    const QSqlRecord record = tableRecord(request, false);
    if (record.isEmpty())
        return QString();

    QSqlDriver *driver = this->driver();

    if (idOnly) {
        const QString identifier = request.identifier();
        if (identifier.isEmpty())
            return QString();

        bool ok = false;
        int id = identifier.toInt(&ok);

        QSqlField primary = record.field(0);
        if (ok)
            primary.setValue(id);
        else
            primary.setValue(identifier);

        const QString fieldName = driver->escapeIdentifier(primary.name(), QSqlDriver::FieldName);
        return QStringLiteral("WHERE %1 = %2").arg(fieldName, driver->formatValue(primary));
    }

    QStringList conditions;

    auto addCondition = [&conditions, &record, &driver](const QString &fieldName, const QString &op, const QVariant &value, bool format = true) {
        QSqlField field = record.field(fieldName);
        if (field.isValid())
            field.setValue(value);
        else
            return;

        QString condition = driver->escapeIdentifier(fieldName, QSqlDriver::FieldName) + ' ' + op + ' ';
        condition.append(format ? driver->formatValue(field) : value.toString());
        conditions.append(condition);
    };

    const QString identifier = request.identifier();
    if (!identifier.isEmpty()) {
        bool ok = false;
        int id = identifier.toInt(&ok);

        if (ok)
            addCondition(record.fieldName(0), "=", id);
        else
            addCondition(record.fieldName(0), "=", identifier);
    }

    const QStringList parameters = request.queryParameterNames();
    for (const QString &parameter : parameters) {
        const QStringList expression = parameter.split('.');
        const QString field = (expression.size() > 0 ? expression.at(0) : QString());
        const QString op = (expression.size() > 1 ? expression.at(1) : QString());

        if (field.isEmpty() || op.isEmpty())
            continue;

        const QVariantList values = request.queryParameterValues(parameter);
        const QVariant value = values.constFirst();

        if (op == "gt")
            addCondition(field, ">", value);
        else if (op == "gte")
            addCondition(field, ">=", value);
        else if (op == "lt")
            addCondition(field, "<", value);
        else if (op == "lte")
            addCondition(field, "<=", value);
        else if (op == "eq")
            addCondition(field, "=", value);
        else if (op == "neq")
            addCondition(field, "!=", value);
        else if (op == "like")
            addCondition(field, "LIKE", value);
        else if (op == "in")
            addCondition(field, "IN", '(' + SqlUtils::formatValues(values, this).join(", ") + ')', false);
        else if (op == "notin")
            addCondition(field, "NOT IN", '(' + SqlUtils::formatValues(values, this).join(", ") + ')', false);
    }

    return (!conditions.isEmpty() ? "WHERE " + conditions.join(" AND ") : QString());
}

QString SqlDatabase::orderByClause(const ServerRequest &request) const
{
    if (!request.hasQueryParameter("sort_by"))
        return QString();

    QStringList orders;
    const QVariantList values = request.queryParameterValues("sort_by");
    for (const QVariant &value : values) {
        const QStringList expression = value.toString().split(",", Qt::SkipEmptyParts);
        if (expression.size() != 2)
            continue;

        const QString field = expression.at(0);
        const QString order = (expression.at(1) == "asc" ? "ASC" : "DESC");
        orders.append(field + ' ' + order);
    }

    return (!orders.isEmpty() ? "ORDER BY " + orders.join(", ") : QString());
}

QString SqlDatabase::limitOffsetClause(const ServerRequest &request, int *page, int *items) const
{
    int currentPage = (page ? *page : 1);
    int perPage = (items ? *items : 50);

    if (request.hasQueryParameter("page"))
        currentPage = request.queryParameterValues("page").constFirst().toInt();

    if (request.hasQueryParameter("items_per_page"))
        perPage = request.queryParameterValues("items_per_page").constFirst().toInt();

    if (page)
        *page = currentPage;

    if (items)
        *items = perPage;

    int offset = (currentPage > 1 ? currentPage - 1 : 0) * perPage;
    return QStringLiteral("LIMIT %1 OFFSET %2").arg(perPage).arg(offset);
}

void SqlDatabase::handleError(const QSqlError &error, ServerResponse *response)
{
    response->setHttpStatusCode(500);

    QJsonObject e;
    e.insert("code", error.nativeErrorCode());
    e.insert("message", error.databaseText());
    e.insert("description", error.driverText());
    e.insert("type", error.type());
    response->setBody(e);

    response->complete();
}

} // namespace RestLink
