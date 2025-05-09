#include "resourcecontroller.h"

#include <RestLink/serverrequest.h>
#include <RestLink/serverresponse.h>
#include <RestLink/httputils.h>

#include <RestLink/debug.h>

namespace RestLink {

AbstractController::AbstractController()
    : m_dataSource(nullptr)
{
}

AbstractController::~AbstractController()
{
}

bool AbstractController::canProcessRequest(const ServerRequest &request) const
{
    return request.endpoint() == endpoint();
}

void *AbstractController::dataSource() const
{
    return m_dataSource;
}

void AbstractController::setDataSource(void *source)
{
    m_dataSource = source;
}

bool AbstractResourceController::canProcessRequest(const ServerRequest &request) const
{
    const QString routeEndpoint = this->endpoint();
    QString requestEndpoint = request.endpoint();

    if (requestEndpoint == routeEndpoint)
        return true;

    requestEndpoint.remove(this->endpoint() + '/');
    return requestEndpoint.count('/') == 0;
}

void AbstractResourceController::processRequest(RequestHandler::Method method, const ServerRequest &request, ServerResponse *response)
{
    if (method == RequestHandler::GetMethod) {
        if (request.identifier().isEmpty())
            index(request, response);
        else
            show(request, response);
        return;
    }

    if (method == RequestHandler::PostMethod) {
        store(request, response);
        return;
    }

    if (method == RequestHandler::PutMethod) {
        update(request, response);
        return;
    }

    if (method == RequestHandler::DeleteMethod) {
        destroy(request, response);
        return;
    }

    restlinkDebug() << "Resource controller for endpoint " << endpoint() << " can't handle request: "
                    << HttpUtils::verbString(method) << ' ' << request.endpoint();
}

} // namespace RestLink
