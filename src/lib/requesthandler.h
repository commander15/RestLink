#ifndef RESTLINK_REQUESTHANDLER_H
#define RESTLINK_REQUESTHANDLER_H

#include <RestLink/global.h>

#include <QtCore/qscopedpointer.h>

namespace RestLink {

class Request;
class Body;
class Response;
class RequestInterceptor;

class RequestHandlerPrivate;
class RESTLINK_EXPORT RequestHandler
{
public:
    enum Method {
        HeadMethod,
        GetMethod,
        PostMethod,
        PutMethod,
        PatchMethod,
        DeleteMethod,

        UnknownMethod = -1
    };

    enum HandlerType {
        NetworkManager,
        ServerHandler,

        UnknownHandler = -1
    };

    virtual ~RequestHandler();

    virtual QString handlerName() const;

    Response *head(const Request &request);
    Response *get(const Request &request);
    Response *post(const Request &request, const Body &body);
    Response *put(const Request &request, const Body &body);
    Response *patch(const Request &request, const Body &body);
    Response *deleteResource(const Request &request);
    Response *send(Method method, const Request &request, const Body &body);

    QList<RequestInterceptor *> requestInterceptors() const;
    void addRequestInterceptor(RequestInterceptor *interceptor);
    void removeRequestInterceptor(RequestInterceptor *interceptor);

    virtual QStringList supportedSchemes() const = 0;
    virtual HandlerType handlerType() const = 0;

protected:
    bool isRequestSupported(const Request &request) const;
    void initResponse(Response *response, const Request &request, Method method);
    virtual Response *sendRequest(Method method, const Request &request, const Body &body) = 0;

    QScopedPointer<RequestHandlerPrivate> d_ptr;
};

} // namespace RestLink

#endif // RESTLINK_REQUESTHANDLER_H
