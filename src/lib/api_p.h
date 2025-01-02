#ifndef RESTLINK_API_P_H
#define RESTLINK_API_P_H

#include "api.h"

#include <RestLink/request.h>

#include "apibase_p.h"

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

    bool hasRemoteRequest(const Request &request) const;
    Request remoteRequest(const Request &request) const;
    QByteArray remoteRequestData(const Request &request) const;

    Api *q;

    QString name;
    QVersionNumber version;
    QUrl url;

    QLocale locale;
    QString userAgent;

    struct RemoteRequest {
        Request request;
        QByteArray data;
    };

    QVector<RemoteRequest> remoteRequests;
};

}


#endif // RESTLINK_API_P_H
