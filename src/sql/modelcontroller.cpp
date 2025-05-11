#include "modelcontroller.h"

#include "model.h"

#include <RestLink/serverrequest.h>
#include <RestLink/serverresponse.h>

#include <QtCore/qjsonarray.h>

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
    QJsonObject json;

    QueryOptions options;

    if (request.hasQueryParameter("limit"))
        options.limit = request.queryParameterValues("limit").constFirst().toInt();
    else
        options.limit = RESTLINK_PAGINATION_LIMIT;

    if (options.limit > 0) {
        int page;

        if (request.hasQueryParameter("page"))
            page = request.queryParameterValues("page").constFirst().toInt();
        else
            page = 1;

        if (page < 1)
            page = 1;

        options.offset = (page - 1) * options.limit;

        double total = Model::count(m_table, options, m_manager);
        json.insert("total", total);
        json.insert("current_page", page);
        json.insert("per_page", options.limit);
        json.insert("last_page", qCeil<double>(total / options.limit));
    }

    QJsonArray result;
    const QList<Model> models = Model::getMulti(m_table, options, m_manager);
    for (const Model &model : models)
        result.append(model.jsonObject());
    json.insert("data", result);

    json.insert("from", options.offset + 1);
    json.insert("to", options.offset + models.count());

    response->setBody(json);
    response->setHttpStatusCode(200);
    response->complete();
}

void ModelController::show(const ServerRequest &request, ServerResponse *response)
{
    Model model(request.resource(), m_manager);

    if (model.get(request.identifier().toInt())) {
        response->setBody(model.jsonObject());
        response->setHttpStatusCode(200);
    } else {
        response->setHttpStatusCode(404);
    }

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

void ModelController::processRequest(RequestHandler::Method method, const ServerRequest &request, ServerResponse *response)
{
    m_endpoint = request.endpoint();
    m_table = request.resource();
    AbstractResourceController::processRequest(method, request, response);
}

} // namespace Sql
} // namespace RestLink
