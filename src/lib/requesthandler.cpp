#include "requesthandler.h"

#include <RestLink/request.h>
#include <RestLink/pathparameter.h>
#include <RestLink/queryparameter.h>
#include <RestLink/response.h>
#include <RestLink/body.h>

#include <QtCore/qurl.h>
#include <QtCore/qurlquery.h>

namespace RestLink {

RequestHandler::~RequestHandler()
{
}

Response *RequestHandler::head(const Request &request, Api *api)
{
    return send(Api::HeadOperation, request, Body(), api);
}

Response *RequestHandler::get(const Request &request, Api *api)
{
    return send(Api::GetOperation, request, Body(), api);
}

Response *RequestHandler::post(const Request &request, const Body &body, Api *api)
{
    return send(Api::PostOperation, request, body, api);
}

Response *RequestHandler::put(const Request &request, const Body &body, Api *api)
{
    return send(Api::PutOperation, request, body, api);
}

Response *RequestHandler::patch(const Request &request, const Body &body, Api *api)
{
    return send(Api::PatchOperation, request, body, api);
}

Response *RequestHandler::deleteResource(const Request &request, Api *api)
{
    return send(Api::DeleteOperation, request, Body(), api);
}

Response *RequestHandler::send(ApiBase::Operation operation, const Request &request, const Body &body, Api *api)
{
    if (isApiSupported(api)) {
        Response *response = sendRequest(operation, request, body, api);
        if (response)
            response->setRequest(request);
        return response;
    } else {
        return nullptr;
    }
}

Response *RequestHandler::send(ApiBase::Operation operation, const Request &request, const Body &body, ApiBase *api)
{
    if (api->inherits("RestLink::Api"))
        return send(operation, request, body, static_cast<Api *>(api));
    else
        return nullptr;
}

bool RequestHandler::isApiSupported(ApiBase *api) const
{
    return supportedSchemes().contains(api->url().scheme());
}

QUrl RequestHandler::generateUrl(const Request &request, ApiBase *api, UrlContext context) const
{
    QUrl url = api->url();

    QString urlPath = url.path() + request.urlPath();
    url.setPath(urlPath);

    QUrlQuery urlQuery(url.query());
    for (const QueryParameter &param : request.queryParameters()) {
        if (context == LogContext && param.hasFlag(Parameter::Secret))
            continue;

        if (param.values().isEmpty()) {
            if (param.hasFlag(Parameter::Locale)) {
                const QString language = api->locale().name();
                urlQuery.addQueryItem(param.name(), language.section('_', 0, 0));
            }

            continue;
        }

        for (const QVariant &value : param.values())
            urlQuery.addQueryItem(param.name(), value.toString());
    }
    url.setQuery(urlQuery);

    return url;
}

} // namespace RestLink
