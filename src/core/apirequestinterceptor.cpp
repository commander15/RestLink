#include "apirequestinterceptor.h"

#include <RestLink/debug.h>
#include <RestLink/apirequest.h>

#include <QtNetwork/qnetworkrequest.h>

namespace RestLink {

ApiRequestInterceptor::~ApiRequestInterceptor()
{
}

QNetworkRequest ApiRequestInterceptor::interceptNetworkRequest(const QNetworkRequest &request, const void *data, int dataType)
{
    Q_UNUSED(data);
    Q_UNUSED(dataType);
    return request;
}

ApiRequest ApiLogRequestInterceptor::interceptApiRequest(const ApiRequest &request, const void *data, int dataType)
{
    Q_UNUSED(data);
    Q_UNUSED(dataType);
    return request;
}

QNetworkRequest ApiLogRequestInterceptor::interceptNetworkRequest(const QNetworkRequest &request, const void *data, int dataType)
{
    restlinkInfo() << request.url().toString();
    return request;
}

}
