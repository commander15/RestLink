#include "modelcontroller.h"

#include <api.h>
#include <data/model.h>
#include <utils/jsonutils.h>

#include <QtCore/qjsonarray.h>

#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlerror.h>
#include <QtSql/qsqlquery.h>

#include <RestLink/serverrequest.h>
#include <RestLink/serverresponse.h>

#define PARAM_WITH_RELATIONS "with_relations"
#define PARAM_PAGE           "page"
#define PARAM_LIMIT          "limit"

namespace RestLink {
namespace Sql {

ModelController::ModelController()
    : m_api(nullptr)
{
}

QString ModelController::endpoint() const
{
    return m_endpoint;
}

void ModelController::init(const ServerRequest &request, Api *api)
{
    m_endpoint = request.endpoint();
    m_resource = api->resourceInfo(request.resource());
    m_api = api;
}

void ModelController::index(const ServerRequest &request, ServerResponse *response)
{
    QueryOptions options;

    options.withRelations = requestedRelations(request, requestedResource(request));

    if (request.hasQueryParameter(PARAM_LIMIT))
        options.limit = request.queryParameterValues(PARAM_LIMIT).constFirst().toInt();
    else
        options.limit = RESTLINK_PAGINATION_LIMIT;

    int page = 1;
    if (options.limit > 0) {
        if (request.hasQueryParameter(PARAM_PAGE))
            page = request.queryParameterValues(PARAM_PAGE).constFirst().toInt();

        if (page < 1)
            page = 1;

        options.offset = (page - 1) * options.limit;
    }

    QJsonObject json;
    QJsonArray result;
    int count;

    QSqlQuery query(m_api->database());
    const QList<Model> models = Model::getMulti(m_resource, options, m_api, &query);

    QSqlError::ErrorType sqlErrorType = query.lastError().type();
    if (sqlErrorType == QSqlError::NoError)
        goto success;
    else
        goto error;

success:
    for (const Model &model : models)
        result.append(model.jsonObject());
    count = Model::count(m_resource, options, m_api);

    json.insert("data", result);
    json.insert("from", options.offset + 1);
    json.insert("to", options.offset + models.count());
    json.insert("total", count);
    json.insert("current_page", page);
    json.insert("per_page", options.limit);
    json.insert("last_page", options.limit > 0 ? qCeil<double>(count / double(options.limit)) : 1);

    response->setHttpStatusCode(200);
    response->setBody(json);
    response->complete();
    return;

error:
    json = JsonUtils::objectFromQuery(query);
    response->setHttpStatusCode(httpStatusCodeFromSqlError(sqlErrorType));
    response->setBody(json);
    response->complete();
    return;
}

void ModelController::show(const ServerRequest &request, ServerResponse *response)
{
    Model model = requestModel(request);
    if (!model.get())
        goto error;

    if (!model.load(requestedRelations(request, requestedResource(request))))
        goto error;

    goto success;

success:
    response->setHttpStatusCode(200);
    response->setBody(model.jsonObject());
    response->complete();
    return;

error:
    response->setHttpStatusCode(httpStatusCodeFromSqlError(model.lastError()));
    response->setBody(model.lastError());
    response->complete();
    return;
}

void ModelController::update(const ServerRequest &request, ServerResponse *response)
{
    Model model = requestModel(request);
    model.fill(request.body().jsonObject());
    if (model.update())
        goto success;
    else
        goto error;

success:
    response->setHttpStatusCode(200);
    response->setBody(model.jsonObject());
    response->complete();
    return;

error:
    response->setHttpStatusCode(httpStatusCodeFromSqlError(model.lastError()));
    response->setBody(model.lastError());
    response->complete();
    return;
}

void ModelController::store(const ServerRequest &request, ServerResponse *response)
{
    Model model = requestModel(request);
    model.fill(request.body().jsonObject());
    if (model.insert())
        goto success;
    else
        goto error;

success:
    response->setHttpStatusCode(200);
    response->complete();
    return;

error:
    response->setBody(model.lastQuery());
    response->setHttpStatusCode(404);
    response->complete();
    return;
}

void ModelController::destroy(const ServerRequest &request, ServerResponse *response)
{
    Model model = requestModel(request);

    if (!model.get(request.identifier().toInt())) {
        response->setHttpStatusCode(404);
        response->complete();
        return;
    }

    if (!model.deleteData()) {
        response->setHttpStatusCode(httpStatusCodeFromSqlError(model.lastError()));
        response->setBody(model.lastError());
        response->complete();
        return;
    }

    response->setHttpStatusCode(200);
    response->complete();
}

bool ModelController::canProcessRequest(const ServerRequest &request) const
{
    if (!m_resource.isValid())
        return false;

    switch (request.method()) {
    case AbstractRequestHandler::GetMethod:
    case AbstractRequestHandler::PostMethod:
    case AbstractRequestHandler::PutMethod:
    case AbstractRequestHandler::DeleteMethod:
        break;

    default:
        return false;
    }

    return true;
}

void ModelController::processRequest(const ServerRequest &request, ServerResponse *response)
{
    QSqlDatabase db = m_api->database();

    if (!db.isOpen()) {
        QJsonObject body = JsonUtils::objectFromError(db.lastError());
        body.insert("connection", db.connectionName());

        response->setHttpStatusCode(httpStatusCodeFromSqlError(db.lastError().type()));
        response->setBody(body);
        response->complete();
        return;
    }

    bool transactionStarted = false;
    if (request.method() != AbstractRequestHandler::GetMethod)
        transactionStarted = db.transaction();

    AbstractResourceController::processRequest(request, response);

    if (transactionStarted) {
        if (response->isSuccess())
            db.commit();
        else
            db.rollback();
    }
}

Model ModelController::requestModel(const ServerRequest &request) const
{
    Model model(request.resource(), m_api);
    model.setPrimary(request.identifier());
    return model;
}

ResourceInfo ModelController::requestedResource(const ServerRequest &request) const
{
    Q_UNUSED(request);
    return m_resource;
}

QStringList ModelController::requestedRelations(const ServerRequest &request, const ResourceInfo &resource) const
{
    QStringList relations = resource.relationNames();

    if (request.hasQueryParameter(PARAM_WITH_RELATIONS) && request.queryParameterValues(PARAM_WITH_RELATIONS).constFirst().toBool())
        return relations;

    relations.removeIf([&request](const QString &relation) {
        if (!request.hasQueryParameter("with_" + relation))
            return true;
        else
            return !request.queryParameterValues("with_" + relation).constFirst().toBool();
    });

    return relations;
}

int ModelController::httpStatusCodeFromSqlError(const QJsonObject &error)
{
    return 500;
}

int ModelController::httpStatusCodeFromSqlError(int type)
{
    switch (type) {
    case QSqlError::ConnectionError:
        return 503;

    case QSqlError::StatementError:
    case QSqlError::TransactionError:
        return 404;

    default:
        return 500;
    }
}

} // namespace Sql
} // namespace RestLink
