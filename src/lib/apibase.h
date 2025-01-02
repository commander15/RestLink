#ifndef RESTLINK_APIBASE_H
#define RESTLINK_APIBASE_H

#include <RestLink/global.h>
#include <RestLink/requestinterface.h>

#include <QtCore/qobject.h>

class QAbstractNetworkCache;
class QNetworkCookieJar;
class QNetworkProxy;
class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;

namespace RestLink {

class Request;
class Body;
class Response;
class RequestInterceptor;

typedef void(ApiRunCallback)(Response *);

class ApiBasePrivate;
class RESTLINK_EXPORT ApiBase : public QObject, public RequestInterface
{
    Q_OBJECT

public:
    enum Operation {
        HeadOperation,
        GetOperation,
        PostOperation,
        PutOperation,
        PatchOperation,
        DeleteOperation,

        UnknownOperation = 0
    };
    Q_ENUM(Operation)

    virtual ~ApiBase();

    virtual QUrl url() const = 0;

    virtual QLocale locale() const;

    void head(const Request &request, std::function<ApiRunCallback> callback);
    Response *head(const Request &request);

    void get(const Request &request, std::function<ApiRunCallback> callback);
    Response *get(const Request &request);

    void post(const Request &request, const Body &body, std::function<ApiRunCallback> callback);
    Response *post(const Request &request, const Body &body);

    void put(const Request &request, const Body &body, std::function<ApiRunCallback> callback);
    Response *put(const Request &request, const Body &body);

    void patch(const Request &request, const Body &body, std::function<ApiRunCallback> callback);
    Response *patch(const Request &request, const Body &body);

    void deleteResource(const Request &request, std::function<ApiRunCallback> callback);
    Response *deleteResource(const Request &request);

    virtual QString userAgent() const;

    QList<RequestInterceptor *> requestInterceptors() const;
    void addRequestInterceptor(RequestInterceptor *interceptor);
    void removeRequestInterceptor(RequestInterceptor *interceptor);

    QAbstractNetworkCache *cache() const;
    void setCache(QAbstractNetworkCache *cache);

    QNetworkCookieJar *cookieJar() const;
    void setCookieJar(QNetworkCookieJar *jar);

    QNetworkProxy proxy() const;
    void setProxy(const QNetworkProxy &proxy);

    QNetworkAccessManager *networkManager() const;
    void setNetworkManager(QNetworkAccessManager *manager);

protected:
    ApiBase(ApiBasePrivate *d, QObject *parent);

    virtual Response *createResponse(const Request &request, QNetworkReply *netReply) = 0;
    virtual QNetworkRequest createNetworkRequest(const Request &request, const Body &body, Operation operation);
    virtual QNetworkReply *createNetworkReply(const QNetworkRequest &request, const Body &body, Operation operation);

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
