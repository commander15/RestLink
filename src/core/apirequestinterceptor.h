#ifndef APIREQUESTINTERCEPTOR_H
#define APIREQUESTINTERCEPTOR_H

#include <RestLink/global.h>

class QNetworkRequest;

namespace RestLink {
class ApiRequest;

class RESTLINK_EXPORT ApiRequestInterceptor
{
public:
    virtual ~ApiRequestInterceptor();

    virtual ApiRequest interceptApiRequest(const ApiRequest &request, const void *data, int dataType) = 0;
    virtual QNetworkRequest interceptNetworkRequest(const QNetworkRequest &request, const void *data, int dataType);
};

class RESTLINK_EXPORT ApiLogRequestInterceptor : public ApiRequestInterceptor
{
public:
    ApiRequest interceptApiRequest(const ApiRequest &request, const void *data, int dataType) override;
    QNetworkRequest interceptNetworkRequest(const QNetworkRequest &request, const void *data, int dataType) override;
};

}

#endif // APIREQUESTINTERCEPTOR_H
