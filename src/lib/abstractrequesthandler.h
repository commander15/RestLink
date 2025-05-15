#ifndef RESTLINK_ABSTRACTREQUESTHANDLER_H
#define RESTLINK_ABSTRACTREQUESTHANDLER_H

#include <RestLink/global.h>

#include <QtCore/qscopedpointer.h>

namespace RestLink {

class Request;
class Body;
class Response;
class AbstractRequestInterceptor;

class AbstractRequestHandlerPrivate;
class RESTLINK_EXPORT AbstractRequestHandler
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

    virtual ~AbstractRequestHandler();

    virtual QString handlerName() const;

    Response *head(const Request &request);
    Response *get(const Request &request);
    Response *post(const Request &request, const Body &body);
    Response *put(const Request &request, const Body &body);
    Response *patch(const Request &request, const Body &body);
    Response *deleteResource(const Request &request);
    Response *send(Method method, const Request &request, const Body &body);

    QList<AbstractRequestInterceptor *> requestInterceptors() const;
    void addRequestInterceptor(AbstractRequestInterceptor *interceptor);
    void removeRequestInterceptor(AbstractRequestInterceptor *interceptor);

    virtual QStringList supportedSchemes() const = 0;
    virtual HandlerType handlerType() const = 0;

protected:
    AbstractRequestHandler();

    bool isRequestSupported(const Request &request) const;
    void initResponse(Response *response, const Request &request, Method method);
    virtual Response *sendRequest(Method method, const Request &request, const Body &body) = 0;

    QScopedPointer<AbstractRequestHandlerPrivate> d_ptr;
};

} // namespace RestLink

#endif // RESTLINK_ABSTRACTREQUESTHANDLER_H
