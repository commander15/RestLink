#ifndef RESTLINK_REQUESTHANDLER_H
#define RESTLINK_REQUESTHANDLER_H

#include <RestLink/global.h>
#include <RestLink/api.h>

namespace RestLink {

class RESTLINK_EXPORT RequestHandler
{
public:
    virtual ~RequestHandler();

    Response *head(const Request &request, Api *api);
    Response *get(const Request &request, Api *api);
    Response *post(const Request &request, const Body &body, Api *api);
    Response *put(const Request &request, const Body &body, Api *api);
    Response *patch(const Request &request, const Body &body, Api *api);
    Response *deleteResource(const Request &request, Api *api);

    Response *send(Api::Operation operation, const Request &request, const Body &body, Api *api);
    Response *send(Api::Operation operation, const Request &request, const Body &body, ApiBase *api);

    virtual QStringList supportedSchemes() const = 0;

protected:
    enum UrlContext {
        LogContext,
        RequestContext
    };

    bool isApiSupported(ApiBase *api) const;

    QUrl generateUrl(const Request &request, ApiBase *api, UrlContext context) const;

    virtual Response *sendRequest(Api::Operation operation, const Request &request, const Body &body, Api *api) = 0;

    friend class ApiBase;
};

} // namespace RestLink

#endif // RESTLINK_REQUESTHANDLER_H
