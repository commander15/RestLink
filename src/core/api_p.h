#ifndef RESTLINK_API_P_H
#define RESTLINK_API_P_H

#include "api.h"

#include <RestLink/apirequest.h>

#include <RestLink/private/apibase_p.h>

namespace RestLink {

class ApiPrivate : public ApiBasePrivate
{
public:
    enum ParameterContext {
        UrlContext,
        HeaderContext
    };

    ApiPrivate(Api *qq);

    void registerNetworkManager(QNetworkAccessManager *manager);

    bool hasRemoteRequest(const ApiRequest &request) const;
    ApiRequest remoteRequest(const ApiRequest &request) const;
    QByteArray remoteRequestData(const ApiRequest &request) const;

    Api *q;

    QString name;
    QVersionNumber version;
    QUrl url;
    QVector<ApiRequestParameter> parameters;

    QLocale locale;
    QString userAgent;

    struct RemoteRequest {
        ApiRequest request;
        QByteArray data;
    };

    QVector<RemoteRequest> remoteRequests;
};

}


#endif // RESTLINK_API_P_H
