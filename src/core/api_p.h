#ifndef RESTLINK_API_P_H
#define RESTLINK_API_P_H

#include "api.h"

namespace RestLink {

class ApiPrivate
{
public:
    enum ParameterContext {
        UrlContext,
        HeaderContext
    };

    ApiPrivate(Api *qq);
    ~ApiPrivate();

    void registerNetworkManager(QNetworkAccessManager *manager);

    void logRequest(const ApiRequest &request);

    QUrl requestUrl(const ApiRequest &request, bool includeSecrets = true) const;
    QList<ApiRequestParameter> requestParameters(const ApiRequest &request, ParameterContext context) const;
    ApiRequest remoteRequest(const ApiRequest &request) const;

    bool isUseableParameter(const ApiRequestParameter &parameter, bool secret = true) const;
    bool isParameterMatchContext(const ApiRequestParameter &parameter, ParameterContext context) const;

    static QByteArray httpVerbFromRequestVerb(int verb);

    Api *q;

    QString name;
    int version;
    QUrl url;

    QString userAgent;

    QVector<ApiRequestParameter> parameters;
    QVector<ApiRequest> requests;

    QNetworkAccessManager *netMan;
};

}


#endif // RESTLINK_API_P_H
