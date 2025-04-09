#include "requesthandler.h"

#include <RestLink/debug.h>
#include <RestLink/request.h>
#include <RestLink/body.h>
#include <RestLink/response.h>
#include <RestLink/httputils.h>

namespace RestLink {

RequestHandler::~RequestHandler()
{
}

QString RequestHandler::handlerName() const
{
    switch (handlerType()) {
    case NetworkManager:
        return QStringLiteral("NetworkManager");

    case ServerHandler:
        return QStringLiteral("UnknownServer");

    default:
        return QStringLiteral("UnknownHandler");
    };
}

Response *RequestHandler::head(const Request &request)
{
    return send(Api::HeadOperation, request, Body());
}

Response *RequestHandler::get(const Request &request)
{
    return send(Api::GetOperation, request, Body());
}

Response *RequestHandler::post(const Request &request, const Body &body)
{
    return send(Api::PostOperation, request, body);
}

Response *RequestHandler::put(const Request &request, const Body &body)
{
    return send(Api::PutOperation, request, body);
}

Response *RequestHandler::patch(const Request &request, const Body &body)
{
    return send(Api::PatchOperation, request, body);
}

Response *RequestHandler::deleteResource(const Request &request)
{
    return send(Api::DeleteOperation, request, Body());
}

Response *RequestHandler::send(ApiBase::Operation operation, const Request &request, const Body &body)
{
    if (!isRequestSupported(request)) {
        restlinkWarning() << handlerName() << ": trying to send an unsupported request !";
        return nullptr;
    }

    return sendRequest(operation, request, body);
}

bool RequestHandler::isRequestSupported(const Request &request) const
{
    return supportedSchemes().contains(request.baseUrl().scheme());
}

void RequestHandler::initResponse(Response *response, const Request &request, ApiBase::Operation operation)
{
    Q_UNUSED(operation);
    response->setRequest(request);
}

} // namespace RestLink
