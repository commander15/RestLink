#ifndef RESTLINK_APIBASE_H
#define RESTLINK_APIBASE_H

#include <RestLink/global.h>

#include <QtCore/qobject.h>

class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;
class QUrl;
class QUrlQuery;

namespace RestLink {
class ApiRequest;
class ApiRequestParameter;
class ApiReply;

class ApiBasePrivate;
class RESTLINK_EXPORT ApiBase : public QObject
{
public:
    ApiBase(QObject *parent = nullptr);
    ApiBase(QNetworkAccessManager *networkManager, QObject *parent = nullptr);
    virtual ~ApiBase();

    virtual QUrl apiUrl() const = 0;
    virtual QList<ApiRequestParameter> apiParameters() const;

    virtual QString userAgent() const;

    ApiReply *exec(const ApiRequest &request);

    QNetworkAccessManager *networkAccessManager() const;

protected:
    ApiBase(ApiBasePrivate *dd, QObject *parent = nullptr);

    QNetworkRequest createNetworkRequest(const ApiRequest &request);
    QNetworkReply *createNetworkReply(const QByteArray &verb, const QNetworkRequest &request, const QByteArray &data, int dataType);

    virtual void setupNetworkRequest(QNetworkRequest *netReq, const ApiRequest &apiReq);
    virtual void setupUrl(QUrl *url, const ApiRequest &apiReq);
    virtual void setupUrlQuery(QUrlQuery *url, const ApiRequest &apiReq);

    QScopedPointer<ApiBasePrivate> d;

    friend class ApiBasePrivate;
};

}

#endif // RESTLINK_APIBASE_H
