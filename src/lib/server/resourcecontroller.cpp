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

void *AbstractController::dataSource() const
{
    return m_dataSource;
}

void AbstractController::setDataSource(void *source)
{
    m_dataSource = source;
}

bool AbstractController::canProcessRequest(const ServerRequest &request) const
{
    return request.endpoint() == endpoint();
}

bool AbstractResourceController::canProcessRequest(const ServerRequest &request) const
{
    QString endpoint = request.endpoint();

    if (endpoint == this->endpoint())
        return true;

    endpoint.remove(this->endpoint() + '/');
    return endpoint.count('/') == 0;
}

void AbstractResourceController::processRequest(ApiBase::Operation operation, const ServerRequest &request, ServerResponse *response)
{
    if (operation == ApiBase::GetOperation) {
        if (request.identifier().isEmpty())
            index(request, response);
        else
            show(request, response);
        return;
    }

    if (operation == ApiBase::PostOperation) {
        store(request, response);
        return;
    }

    if (operation == ApiBase::PutOperation) {
        update(request, response);
        return;
    }

    if (operation == ApiBase::DeleteOperation) {
        destroy(request, response);
        return;
    }

    restlinkDebug() << "Resource controller for endpoint " << endpoint() << " can't handle request: "
                    << HttpUtils::verbString(operation) << ' ' << request.endpoint();
}

} // namespace RestLink
