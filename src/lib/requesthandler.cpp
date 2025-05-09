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
    return send(HeadMethod, request, Body());
}

Response *RequestHandler::get(const Request &request)
{
    return send(GetMethod, request, Body());
}

Response *RequestHandler::post(const Request &request, const Body &body)
{
    return send(PostMethod, request, body);
}

Response *RequestHandler::put(const Request &request, const Body &body)
{
    return send(PutMethod, request, body);
}

Response *RequestHandler::patch(const Request &request, const Body &body)
{
    return send(PatchMethod, request, body);
}

Response *RequestHandler::deleteResource(const Request &request)
{
    return send(DeleteMethod, request, Body());
}

Response *RequestHandler::send(Method method, const Request &request, const Body &body)
{
    if (!isRequestSupported(request)) {
        restlinkWarning() << handlerName() << ": trying to send an unsupported request !";
        return nullptr;
    }

    return sendRequest(method, request, body);
}

bool RequestHandler::isRequestSupported(const Request &request) const
{
    return supportedSchemes().contains(request.baseUrl().scheme());
}

void RequestHandler::initResponse(Response *response, const Request &request, Method method)
{
    Q_UNUSED(method);
    response->setRequest(request);
}

} // namespace RestLink
