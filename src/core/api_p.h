#ifndef RESTLINK_API_P_H
#define RESTLINK_API_P_H

#include "api.h"

namespace RestLink {

class ApiPrivate
{
public:
    ApiPrivate(Api *qq);
    ~ApiPrivate();

    void registerNetworkManager(QNetworkAccessManager *manager);

    static QByteArray httpVerbFromRequestVerb(int verb);

    Api *q;

    QString name;
    int version;
    QUrl url;

    QString userAgent;

    QVector<ApiRequestParameter> parameters;

    QNetworkAccessManager *netMan;
};

}


#endif // RESTLINK_API_P_H
