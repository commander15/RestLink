#include "modelcontroller.h"

#include "jsonutils.h"
#include "model.h"
#include "api.h"

#include <RestLink/serverrequest.h>
#include <RestLink/serverresponse.h>

#include <QtCore/qjsonarray.h>

#include <QtSql/qsqldatabase.h>
#include <qsqlerror.h>
#include <qsqlquery.h>

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

void ModelController::setApi(Api *api)
{
    m_api = api;
}

void ModelController::index(const ServerRequest &request, ServerResponse *response)
{
    QueryOptions options;

    if (request.hasQueryParameter(PARAM_WITH_RELATIONS))
        options.withRelations = request.queryParameterValues(PARAM_WITH_RELATIONS).constFirst().toBool();

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

    QSqlQuery query;
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
    Model model = currentModel(request);
    if (!model.get())
        goto error;

    if (request.hasQueryParameter(PARAM_WITH_RELATIONS) && request.queryParameterValues(PARAM_WITH_RELATIONS).constFirst().toBool())
        model.loadAll();

    goto success;

success:
    response->setHttpStatusCode(200);
    response->setBody(model.jsonObject());
    response->complete();
    return;

error:
    response->setHttpStatusCode(httpStatusCodeFromSqlError(model.lastQuery().lastError().type()));
    response->setBody(JsonUtils::objectFromQuery(model.lastQuery()));
    response->complete();
    return;
}

void ModelController::update(const ServerRequest &request, ServerResponse *response)
{
    Model model = currentModel(request);
    model.fill(request.body().jsonObject());
    if (model.update())
        goto success;
    else
        goto error;

success:
    response->setBody(model.jsonObject());
    response->setHttpStatusCode(200);
    response->complete();
    return;

error:
    response->setHttpStatusCode(httpStatusCodeFromSqlError(model.lastQuery().lastError().type()));
    response->setBody(JsonUtils::objectFromQuery(model.lastQuery()));
    response->complete();
    return;
}

void ModelController::store(const ServerRequest &request, ServerResponse *response)
{
    Model model = currentModel(request);
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
    response->setBody(JsonUtils::objectFromQuery(model.lastQuery()));
    response->setHttpStatusCode(404);
    response->complete();
    return;
}

void ModelController::destroy(const ServerRequest &request, ServerResponse *response)
{
    Model model = currentModel(request);

    if (!model.get(request.identifier().toInt())) {
        response->setHttpStatusCode(404);
        response->complete();
        return;
    }

    if (!model.deleteData()) {
        response->setHttpStatusCode(httpStatusCodeFromSqlError(model.lastQuery().lastError().type()));
        response->setBody(JsonUtils::objectFromQuery(model.lastQuery()));
        response->complete();
        return;
    }

    response->setHttpStatusCode(model.deleteData() ? 200 : 500);
    response->complete();
}

bool ModelController::canProcessRequest(const ServerRequest &request) const
{
    if (!m_api)
        return false;

    switch (request.method()) {
    case RequestHandler::GetMethod:
    case RequestHandler::PostMethod:
    case RequestHandler::PutMethod:
    case RequestHandler::DeleteMethod:
        break;

    default:
        return false;
    }

    const ResourceInfo info = m_api->resourceInfo(request.resource());
    return info.isValid();
}

void ModelController::processRequest(const ServerRequest &request, ServerResponse *response)
{
    m_endpoint = request.endpoint();
    m_resource = request.resource();

    QSqlDatabase db = m_api->database();
    db.transaction();

    AbstractResourceController::processRequest(request, response);

    if (response->isSuccess())
        db.commit();
    else
        db.rollback();
}

Model ModelController::currentModel(const ServerRequest &request) const
{
    Model model(request.resource(), m_api);

    QVariant id = request.identifier();
    if (id.canConvert<qint64>())
        id.convert(QMetaType::fromType<qint64>());

    model.setPrimary(id);
    return model;
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
