#include "api.h"

#include "jsonutils.h"
#include "modelmanager.h"

#include <QtCore/qfile.h>
#include <qjsonarray.h>
#include <qsqlerror.h>
#include <qsqlquery.h>

namespace RestLink {
namespace Sql {

Api::Api(QObject *parent)
    : RestLink::Server(Synchronous, parent)
{
}

Api::~Api()
{
    ModelManager::cleanupManagers();
}

QString Api::handlerName() const
{
    return QStringLiteral("SQL-API");
}

QStringList Api::supportedSchemes() const
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

bool Api::init()
{
    return true;
}

void Api::cleanup()
{
    // No-op
}

bool Api::maintain()
{
    return true;
}

void Api::processRequest(const ServerRequest &request, ServerResponse *response)
{
    ModelManager *manager = ModelManager::manager(request.baseUrl());
    if (!manager) {
        response->setHttpStatusCode(404);
        response->complete();
        return;
    }

    if (request.hasQueryParameter("configuration-file")) {
        QFile file(request.queryParameterValues("configuration-file").constFirst().toString());
        if (file.open(QIODevice::ReadOnly)) {
            QJsonObject config = JsonUtils::objectFromRawData(file.readAll(), QJsonObject());
            if (!config.isEmpty())
                manager->configure(config);
        }
    }

    if (request.endpoint() == "/configuration") {
        processConfigurationRequest(request, response, manager);
        return;
    }

    if (request.endpoint() == "/tables") {
        processTablesRequest(request, response, manager);
        return;
    }

    if (request.endpoint() == "/query") {
        processQueryRequest(request, response, manager);
        return;
    }

    m_defaultController.setManager(manager);
    if (m_defaultController.canProcessRequest(request)) {
        m_defaultController.processRequest(request, response);
        return;
    }

    response->setHttpStatusCode(404);
    response->complete();
}

void Api::processConfigurationRequest(const ServerRequest &request, ServerResponse *response, ModelManager *manager)
{
    switch (request.method()) {
    case PostMethod:
        manager->configure(JsonUtils::objectFromRawData(request.body().data(), QJsonObject()));

    case GetMethod:
        response->setBody(manager->configuration());
        response->setHttpStatusCode(200);
        break;

    default:
        response->setHttpStatusCode(404);
        break;
    }

    response->complete();
}

void Api::processTablesRequest(const ServerRequest &request, ServerResponse *response, ModelManager *manager)
{
    bool includeMetaData = true;
    if (request.hasQueryParameter("meta"))
        includeMetaData = request.queryParameterValues("meta").constFirst().toBool();

    response->setHttpStatusCode(200);
    response->setBody(manager->modelDefinitions(includeMetaData));
    response->complete();
}

void Api::processQueryRequest(const ServerRequest &request, ServerResponse *response, ModelManager *manager)
{
    response->setBody(request.body());
    response->complete();
    return;

    QSqlQuery query(manager->database());
    query.setForwardOnly(true);

    auto process = [&query](const QByteArray &statement, bool *success) -> QJsonObject {
        QJsonObject body;

        if (!query.exec(statement)) {
            QSqlError error = query.lastError();
            body.insert("query", QString::fromUtf8(statement));
            body.insert("message", error.text());
            body.insert("error_code", error.nativeErrorCode());
            *success = false;
            return body;
        }

        QJsonArray data;
        while (query.next())
            data.append(JsonUtils::objectFromRecord(query.record()));

        body.insert("last_insert_id", QJsonValue::fromVariant(query.lastInsertId()));
        body.insert("num_rows_affected", query.numRowsAffected());
        body.insert("data", data);
        *success = true;
        return body;
    };

    QByteArrayList statements;
    QByteArrayList input = request.body().data().trimmed().split('\n');

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
        response->setHttpStatusCode(successes > 0 ? 200 : 401);
        response->complete();
        return;
    }

    response->setHttpStatusCode(200);
    response->complete();
}

} // namespace Sql
} // namespace RestLink
