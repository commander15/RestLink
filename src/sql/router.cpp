#include "router.h"

#include "jsonutils.h"
#include "api.h"
#include "resourceinfo.h"

#include <QtCore/qfile.h>
#include <QtCore/qjsonarray.h>

#include <QtSql/qsqlerror.h>
#include <QtSql/qsqlquery.h>

#include <RestLink/httputils.h>

namespace RestLink {
namespace Sql {

Router::Router(QObject *parent)
    : RestLink::Server(Synchronous, parent)
{
}

Router::~Router()
{
    Api::cleanupManagers();
}

QString Router::handlerName() const
{
    return QStringLiteral("SQL");
}

QStringList Router::supportedSchemes() const
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

bool Router::init()
{
    return true;
}

void Router::cleanup()
{
    // No-op
}

bool Router::maintain()
{
    return true;
}

void Router::processStandardRequest(const ServerRequest &request, ServerResponse *response)
{
    Api *api = Api::api(request.baseUrl());
    if (!api) {
        response->setHttpStatusCode(404);
        response->complete();
        return;
    }

    if (request.hasQueryParameter("configuration-file")) {
        QFile file(request.queryParameterValues("configuration-file").constFirst().toString());
        if (file.open(QIODevice::ReadOnly)) {
            QJsonObject config = JsonUtils::objectFromRawData(file.readAll());
            if (!config.isEmpty())
                api->configure(config);
        }
    }

    if (request.endpoint() == "/configuration") {
        processConfigurationRequest(request, response, api);
        return;
    }

    if (request.endpoint() == "/query") {
        processQueryRequest(request, response, api);
        return;
    }

    m_defaultController.setApi(api);
    if (m_defaultController.canProcessRequest(request)) {
        m_defaultController.processRequest(request, response);
        return;
    }

    processUnsupportedRequest(request, response);
}

void Router::processConfigurationRequest(const ServerRequest &request, ServerResponse *response, Api *manager)
{
    switch (request.method()) {
    case PostMethod:
    case PutMethod:
        manager->configure(JsonUtils::objectFromRawData(request.body().toByteArray()));

    case GetMethod:
        response->setHttpStatusCode(200);
        response->setBody(manager->configuration());
        response->complete();
        break;

    case DeleteMethod:
        manager->reset();
        response->setHttpStatusCode(200);
        response->setBody(QJsonObject({ { "message", "configuration reseted successfully" } }));
        response->complete();
        break;

    default:
        processUnsupportedRequest(request, response);
        break;
    }
}

void Router::processQueryRequest(const ServerRequest &request, ServerResponse *response, Api *manager)
{
    if (request.method() != AbstractRequestHandler::PostMethod) {
        processUnsupportedRequest(request, response);
        return;
    }

    QSqlQuery query(manager->database());
    query.setForwardOnly(true);

    auto process = [&query](const QByteArray &statement, bool *success) -> QJsonObject {
        if (!query.exec(statement)) {
            *success = false;
            return JsonUtils::objectFromQuery(query);
        } else {
            *success = true;
        }

        QJsonArray data;
        while (query.next())
            data.append(JsonUtils::objectFromRecord(query.record(), ResourceInfo()));
        query.finish();

        QJsonObject body;
        body.insert("last_insert_id", QJsonValue::fromVariant(query.lastInsertId()));
        body.insert("num_rows_affected", query.numRowsAffected());
        body.insert("data", data);
        return body;
    };

    QByteArrayList statements;
    QByteArrayList input = request.body().toByteArray().trimmed().split('\n');

    {
        QByteArray statement;
        QByteArray delimiter = ";";
        for (QByteArray &line : input) {
            line = line.trimmed();

            // We skip empty lines
            if (line.isEmpty())
                continue;

            statement.append(line);

            if (line.endsWith(delimiter)) {
                statements.append(statement);
                statement.clear();
            }
        }

        // If there is only one statement without delimiter
        if (statements.isEmpty() && !statement.isEmpty())
            statements.append(statement);
    }

    if (statements.size() == 1) {
        bool success;
        response->setBody(process(statements.constFirst(), &success));
        response->setHttpStatusCode(success ? 200 : 401);
        response->complete();
        return;
    }

    if (statements.size() > 1) {
        int successes = 0;
        bool success;

        QJsonArray results;
        for (const QByteArray &statement : statements) {
            results.append(process(statement, &success));
            successes += (success ? 1 : 0);
        }

        response->setBody(results);
        response->setHttpStatusCode(successes > 0 ? 200 : 500);
        response->complete();
        return;
    }

    response->setHttpStatusCode(200);
    response->complete();
}

} // namespace Sql
} // namespace RestLink
