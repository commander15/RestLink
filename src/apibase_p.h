#ifndef RESTLINK_APIBASE_P_H
#define RESTLINK_APIBASE_P_H

#include "apibase.h"

class QNetworkAccessManager;

namespace RestLink {

class ApiBasePrivate
{
public:
    ApiBasePrivate(ApiBase *qq);
    virtual ~ApiBasePrivate();

    void logRequest(const ApiRequest &request);
    void logRequest(const QNetworkRequest &request);

    ApiBase *q;

    QNetworkAccessManager *netMan;
};

}

#endif // RESTLINK_APIBASE_P_H
