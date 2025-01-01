#ifndef RESTLINK_REQUEST_H
#define RESTLINK_REQUEST_H

#include <RestLink/global.h>
#include <RestLink/requestinterface.h>

#include <QtCore/qshareddata.h>

class QJsonObject;

namespace RestLink {

class RequestData;
class RESTLINK_EXPORT Request : public RequestInterface
{
public:
    Request();
    Request(const char *endpoint);
    Request(const QString &endpoint);
    Request(const Request &other);
    Request(Request &&other);
    ~Request();

    Request &operator=(const Request &other);
    Request &operator=(const Request &&other);

    QString endpoint() const;
    void setEndpoint(const QString &endpoint);

    QString urlPath() const;

    QJsonObject toJsonObject() const;
    static Request fromJsonbject(const QJsonObject &object);

    void swap(Request &other);

    static Request merge(const Request &r1, const Request &r2);

private:
    const QList<PathParameter> *constPathParameters() const override;
    QList<PathParameter> *mutablePathParameters() override;
    const QList<QueryParameter> *constQueryParameters() const override;
    QList<QueryParameter> *mutableQueryParameters() override;
    const QList<Header> *constHeaders() const override;
    QList<Header> *mutableHeaders() override;

    QSharedDataPointer<RequestData> d_ptr;

    friend class ApiBase;
};

}

#endif // RESTLINK_REQUEST_H
