#include "requesthandler.h"

#include <RestLink/request.h>
#include <RestLink/body.h>
#include <RestLink/response.h>

namespace RestLink {

RequestHandler::~RequestHandler()
{
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
    if (isRequestSupported(request)) {
        Response *response = sendRequest(operation, request, body);
        if (response)
            response->setRequest(request);
        return response;
    } else {
        return nullptr;
    }
}

bool RequestHandler::isRequestSupported(const Request &request) const
{
    return supportedSchemes().contains(request.baseUrl().scheme());
}

} // namespace RestLink
