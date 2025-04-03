#ifndef RESTLINK_REQUEST_H
#define RESTLINK_REQUEST_H

#include <RestLink/global.h>
#include <RestLink/requestinterface.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qmetatype.h>

class QJsonObject;
class QHttpHeaders;

namespace RestLink {

class Request;
class Body;
class Response;
class Api;

typedef std::function<void(const Request &, const Body &, void *, Response *)> RequestProcessing;

class RequestData;
class RESTLINK_EXPORT Request : public RequestInterface
{
public:
    enum UrlType {
        PublicUrl,
        SecretUrl
    };

    Request();
    Request(const char *endpoint);
    Request(const QString &endpoint);
    Request(const QUrl &url);
    Request(const RequestProcessing &processing);
    Request(const Request &other);
    Request(Request &&other);
    ~Request();

    Request &operator=(const Request &other);
    Request &operator=(const Request &&other);

    QString endpoint() const;
    void setEndpoint(const QString &endpoint);

    QUrl baseUrl() const;
    void setBaseUrl(const QUrl &url);

    QUrl url(UrlType type = SecretUrl) const;
    QString urlPath() const;

    QHttpHeaders httpHeaders() const;

    RequestProcessing processing() const;
    void setProcessing(RequestProcessing processing);

    Api *api() const;
    void setApi(Api *api);

    QJsonObject toJsonObject() const;
    static Request fromJsonbject(const QJsonObject &object);

    void swap(Request &other);

    static Request merge(const Request &r1, const Request &r2);

private:
    Request(RequestData *d);

    const QList<PathParameter> *constPathParameters() const override;
    QList<PathParameter> *mutablePathParameters() override;
    const QList<QueryParameter> *constQueryParameters() const override;
    QList<QueryParameter> *mutableQueryParameters() override;
    const QList<Header> *constHeaders() const override;
    QList<Header> *mutableHeaders() override;

    QSharedDataPointer<RequestData> d_ptr;

    friend class ApiBase;
    friend class Api;
};

}

Q_DECLARE_METATYPE(RestLink::Request)

#endif // RESTLINK_REQUEST_H
