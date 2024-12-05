#ifndef RESTLINK_APIBASE_H
#define RESTLINK_APIBASE_H

#include <RestLink/global.h>

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>

class QHttpMultiPart;
class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;

namespace RestLink {

class ApiRequest;
class ApiRequestParameter;
class ApiReply;

typedef void(ApiRunCallback)(ApiReply *);

class ApiBasePrivate;
class RESTLINK_EXPORT ApiBase : public QObject
{
    Q_OBJECT

public:
    enum Operation {
        GetOperation,
        PostOperation,
        PutOperation,
        PatchOperation,
        DeleteOperation,

        UnknownOperation = 0
    };
    Q_ENUM(Operation)

    virtual ~ApiBase();

    virtual QUrl apiUrl() const = 0;
    virtual QList<ApiRequestParameter> apiParameters() const = 0;

    void get(const ApiRequest &request, std::function<ApiRunCallback> callback);

    void post(const ApiRequest &request, QIODevice *device, std::function<ApiRunCallback> callback);
    void post(const ApiRequest &request, const QByteArray &data, std::function<ApiRunCallback> callback);
    void post(const ApiRequest &request, QHttpMultiPart *data, std::function<ApiRunCallback> callback);
    void post(const ApiRequest &request, const QJsonValue &data, std::function<ApiRunCallback> callback);

    void put(const ApiRequest &request, QIODevice *device, std::function<ApiRunCallback> callback);
    void put(const ApiRequest &request, const QByteArray &data, std::function<ApiRunCallback> callback);
    void put(const ApiRequest &request, QHttpMultiPart *data, std::function<ApiRunCallback> callback);
    void put(const ApiRequest &request, const QJsonValue &data, std::function<ApiRunCallback> callback);

    void patch(const ApiRequest &request, QIODevice *device, std::function<ApiRunCallback> callback);
    void patch(const ApiRequest &request, const QByteArray &data, std::function<ApiRunCallback> callback);
    void patch(const ApiRequest &request, QHttpMultiPart *data, std::function<ApiRunCallback> callback);
    void patch(const ApiRequest &request, const QJsonValue &data, std::function<ApiRunCallback> callback);

    void deleteResource(const ApiRequest &request, std::function<ApiRunCallback> callback);

    ApiReply *get(const ApiRequest &request);

    ApiReply *post(const ApiRequest &request, QIODevice *device);
    ApiReply *post(const ApiRequest &request, const QByteArray &data);
    ApiReply *post(const ApiRequest &request, QHttpMultiPart *data);
    ApiReply *post(const ApiRequest &request, const QJsonValue &data);

    ApiReply *put(const ApiRequest &request, QIODevice *device);
    ApiReply *put(const ApiRequest &request, const QByteArray &data);
    ApiReply *put(const ApiRequest &request, QHttpMultiPart *data);
    ApiReply *put(const ApiRequest &request, const QJsonValue &data);

    ApiReply *patch(const ApiRequest &request, QIODevice *device);
    ApiReply *patch(const ApiRequest &request, const QByteArray &data);
    ApiReply *patch(const ApiRequest &request, QHttpMultiPart *data);
    ApiReply *patch(const ApiRequest &request, const QJsonValue &data);

    ApiReply *deleteResource(const ApiRequest &request);

    virtual QString userAgent() const = 0;

    QNetworkAccessManager *networkAccessManager() const;
    void setNetworkAccessManager(QNetworkAccessManager *manager);

protected:
    ApiBase(ApiBasePrivate *d, QObject *parent);

    virtual ApiReply *createApiReply(const ApiRequest &request, QNetworkReply *netReply) = 0;
    virtual QNetworkRequest createNetworkRequest(const ApiRequest &request);

    QScopedPointer<ApiBasePrivate> d_ptr;

    friend class ApiBasePrivate;
};

}

#endif // RESTLINK_APIBASE_H
