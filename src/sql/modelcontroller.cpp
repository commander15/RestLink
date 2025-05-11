#include "modelcontroller.h"

#include "model.h"

#include <RestLink/serverrequest.h>
#include <RestLink/serverresponse.h>

#include <QtCore/qjsonarray.h>

#define PARAM_WITH_RELATIONS "with_relations"
#define PARAM_PAGE           "page"
#define PARAM_LIMIT          "limit"

namespace RestLink {
namespace Sql {

ModelController::ModelController()
    : m_manager(nullptr)
{
}

QString ModelController::endpoint() const
{
    return m_endpoint;
}

void ModelController::setManager(ModelManager *manager)
{
    m_manager = manager;
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

    QJsonArray result;
    const QList<Model> models = Model::getMulti(m_table, options, m_manager);
    for (const Model &model : models)
        result.append(model.jsonObject());

    double count = Model::count(m_table, options, m_manager);

    QJsonObject json;
    json.insert("data", result);
    json.insert("from", options.offset + 1);
    json.insert("to", options.offset + models.count());
    json.insert("total", count);
    json.insert("current_page", page);
    json.insert("per_page", options.limit);
    json.insert("last_page", qCeil<double>(count / options.limit));

    response->setBody(json);
    response->setHttpStatusCode(200);
    response->complete();
}

void ModelController::show(const ServerRequest &request, ServerResponse *response)
{
    Model model(request.resource(), m_manager);

    if (!model.get(request.identifier().toInt()))
        goto error;

    if (request.hasQueryParameter(PARAM_WITH_RELATIONS) && request.queryParameterValues(PARAM_WITH_RELATIONS).constFirst().toBool())
        model.loadAll();

    response->setBody(model.jsonObject());
    response->setHttpStatusCode(200);

error:
    response->setHttpStatusCode(404);
    response->complete();
}

void ModelController::update(const ServerRequest &request, ServerResponse *response)
{
}

void ModelController::store(const ServerRequest &request, ServerResponse *response)
{
}

void ModelController::destroy(const ServerRequest &request, ServerResponse *response)
{
    Model model(request.resource(), m_manager);

    if (!model.get(request.identifier().toInt())) {
        response->setHttpStatusCode(404);
        response->complete();
        return;
    }

    response->setHttpStatusCode(model.deleteData() ? 200 : 500);
    response->complete();
}

bool ModelController::canProcessRequest(const ServerRequest &request) const
{
    return m_manager && !request.resource().isEmpty();
}

void ModelController::processRequest(const ServerRequest &request, ServerResponse *response)
{
    m_endpoint = request.endpoint();
    m_table = request.resource();
    AbstractResourceController::processRequest(request, response);
}

} // namespace Sql
} // namespace RestLink
