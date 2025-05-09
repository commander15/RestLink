#ifndef RESTLINK_APIBASE_H
#define RESTLINK_APIBASE_H

#include <RestLink/global.h>
#include <RestLink/requestinterface.h>
#include <RestLink/requesthandler.h>

#include <QtCore/qobject.h>

#include <functional>

namespace RestLink {

class Request;
class Body;
class Response;
class RequestInterceptor;
class NetworkManager;

typedef std::function<void(Response *)> ApiRunCallback;

class ApiBasePrivate;
class RESTLINK_EXPORT ApiBase : public QObject, public RequestInterface
{
    Q_OBJECT

public:
    virtual ~ApiBase();

    virtual QUrl url() const = 0;

    virtual QLocale locale() const;

    void head(const Request &request, const ApiRunCallback &callback);
    Response *head(const Request &request);

    void get(const Request &request, const ApiRunCallback &callback);
    Response *get(const Request &request);

    void post(const Request &request, const Body &body, const ApiRunCallback &callback);
    Response *post(const Request &request, const Body &body);

    void put(const Request &request, const Body &body, const ApiRunCallback &callback);
    Response *put(const Request &request, const Body &body);

    void patch(const Request &request, const Body &body, const ApiRunCallback &callback);
    Response *patch(const Request &request, const Body &body);

    void deleteResource(const Request &request, const ApiRunCallback &callback);
    Response *deleteResource(const Request &request);

    virtual Response *send(RequestHandler::Method method, const Request &request, const Body &body);

    virtual QString userAgent() const;

    QList<RequestInterceptor *> requestInterceptors() const;
    void addRequestInterceptor(RequestInterceptor *interceptor);
    void removeRequestInterceptor(RequestInterceptor *interceptor);

    NetworkManager *networkManager() const;
    void setNetworkManager(NetworkManager *manager);

protected:
    ApiBase(ApiBasePrivate *d, QObject *parent);

    QScopedPointer<ApiBasePrivate> d_ptr;

private:
    const QList<PathParameter> *constPathParameters() const override;
    QList<PathParameter> *mutablePathParameters() override;
    const QList<QueryParameter> *constQueryParameters() const override;
    QList<QueryParameter> *mutableQueryParameters() override;
    const QList<Header> *constHeaders() const override;
    QList<Header> *mutableHeaders() override;

    friend class ApiBasePrivate;
};

}

#endif // RESTLINK_APIBASE_H
