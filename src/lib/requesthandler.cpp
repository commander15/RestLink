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
        const bool log = isLoggingEnabled();

        if (log)
            restlinkInfo() << HttpUtils::verbString(operation) << ' ' << request.url(Request::PublicUrl).toString(QUrl::DecodeReserved);

        Response *response = sendRequest(operation, request, body);
        if (response) {
            response->setRequest(request);
            response->setApi(request.api());

            if (log) {
                QObject::connect(response, &Response::finished, response, [response] {
                    if (response->isSuccess())
                        return;

                    if (response->hasHttpStatusCode())
                        restlinkWarning() << "HTTP " << response->httpStatusCode() << ' ' << response->httpReasonPhrase();
                    else if (response->hasNetworkError())
                        restlinkWarning() << response->networkErrorString();
                    else
                        restlinkWarning() << "Unkown error occured";
                });
            }
        } else if (log) {
            restlinkWarning() << "Response object creation failed, perhaps a plugin related error";
        }

        return response;
    } else {
        return nullptr;
    }
}

bool RequestHandler::isLoggingEnabled() const
{
#ifdef RESTLINK_DEBUG
    return handlerType() == NetworkManager;
#else
    return false;
#endif
}

bool RequestHandler::isRequestSupported(const Request &request) const
{
    return supportedSchemes().contains(request.baseUrl().scheme());
}

} // namespace RestLink
