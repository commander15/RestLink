#include "router.h"
#include "utils/querybuilder.h"
#include "utils/queryrunner.h"

#include <api.h>
#include <meta/endpointinfo.h>
#include <utils/jsonutils.h>

#include <QtCore/qfile.h>
#include <QtCore/qjsonarray.h>

#include <QtSql/qsqlerror.h>
#include <QtSql/qsqlquery.h>

#include <RestLink/serverrequest.h>
#include <RestLink/serverresponse.h>
#include <RestLink/queryparameter.h>
#include <RestLink/httputils.h>

namespace RestLink {
namespace Sql {

Router::Router(QObject *parent)
    : RestLink::AbstractServerWorker(Synchronous, parent)
{
}

Router::~Router()
{
}

bool Router::init()
{
    return true;
}

void Router::cleanup()
{
    Api::cleanupApis();
}

bool Router::maintain()
{
    if (Api::apiCount() >= 5)
        Api::purgeApis(1, true);
    else
        Api::purgeApis(0, false);
    return true;
}

void Router::processStandardRequest(const ServerRequest &request, ServerResponse *response)
{
    Api *api = Api::api(request.baseUrl());
    if (api) {
        api->resetIdleTime();
    } else {
        response->setHttpStatusCode(500);
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

    if (request.endpoint() == "/db-tables") {
        processDatabaseTablesRequest(request, response, api);
        return;
    }

    const EndpointInfo endpoint = api->endpointInfo(request.endpoint());
    if (request.method() == AbstractRequestHandler::GetMethod && endpoint.hasGetQuery()) {
        QVariantHash parameters;
        const QList<QueryParameter> queryParameters = request.queryParameters();
        for (const QueryParameter &parameter : queryParameters)
            parameters.insert(parameter.name(), parameter.value());

        const SqlQueryInfo query = endpoint.getQuery();
        const QStringList queries = query.allFormated(parameters);

        ServerRequest queryRequest(AbstractRequestHandler::PostMethod, request, queries.join(";\n"));
        queryRequest.setEndpoint("/query");
        queryRequest.addQueryParameter("object", query.isObjectQuery());
        processQueryRequest(queryRequest, response, api);
        return;
    }

    m_defaultController.init(request, api);
    if (m_defaultController.canProcessRequest(request)) {
        m_defaultController.processRequest(request, response);
        return;
    }

    processUnsupportedRequest(request, response);
}

void *Router::createDataSource(const ServerRequest &request)
{
    Api *api = Api::api(request.baseUrl());
    return (api ? new QSqlDatabase(api->database()) : nullptr);
}

void Router::clearDataSource(const ServerRequest &request, void *source)
{
    Q_UNUSED(request);

    if (source)
        delete static_cast<QSqlDatabase *>(source);
}

void Router::processConfigurationRequest(const ServerRequest &request, ServerResponse *response, Api *api)
{
    switch (request.method()) {
    case AbstractRequestHandler::PostMethod:
    case AbstractRequestHandler::PutMethod:
        api->configure(JsonUtils::objectFromRawData(request.body().toByteArray()));
        if (request.hasHeader("Connection"))
            api->setConnectionClosable(request.headerValues("Connection").constFirst() == "keep-alive");

    case AbstractRequestHandler::GetMethod:
        response->setHttpStatusCode(200);
        response->setBody(api->configuration());
        response->complete();
        break;

    case AbstractRequestHandler::DeleteMethod:
        api->reset();
        response->setHttpStatusCode(200);
        response->setBody(QJsonObject({ { "message", "configuration reseted successfully" } }));
        response->complete();
        break;

    default:
        processUnsupportedRequest(request, response);
        break;
    }
}

void Router::processDatabaseTablesRequest(const ServerRequest &request, ServerResponse *response, Api *api)
{
    if (request.method() != AbstractRequestHandler::GetMethod) {
        processUnsupportedRequest(request, response);
        return;
    }

    QJsonArray tables = QJsonArray::fromStringList(api->database().tables());

    response->setHttpStatusCode(200);
    response->setBody(tables);
    response->complete();
}

void Router::processQueryRequest(const ServerRequest &request, ServerResponse *response, Api *api)
{
    if (request.method() != AbstractRequestHandler::PostMethod) {
        processUnsupportedRequest(request, response);
        return;
    }

    QSqlQuery query(api->database());
    query.setForwardOnly(true);

    auto process = [&query, api](const QString &statement, bool singleResult, bool *success) -> QJsonObject {
        Query query;
        query.statement = statement;
        query.array = !singleResult;
        return QueryRunner::exec(query, api, success);
    };

    const QString input = request.body().toString().trimmed();
    const QStringList statements = QueryBuilder::statementsFromScript(input);

    bool forceObject = request.hasQueryParameter("object") && request.queryParameterValues("object").constFirst().toBool();
    if (statements.size() == 1 && forceObject) {
        bool success;
        response->setBody(process(statements.constFirst(), forceObject, &success));
        response->setHttpStatusCode(success ? 200 : 401);
        response->complete();
        return;
    }

    if (statements.size() > 1) {
        int successes = 0;
        bool success;

        QJsonArray results;
        for (const QString &statement : statements) {
            results.append(process(statement, false, &success));
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
