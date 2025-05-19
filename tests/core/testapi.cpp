#include "testapi.h"

TestApi::TestApi(QObject *parent)
    : RestLink::Api{parent}
{
}

QNetworkRequest TestApi::generateNetworkRequest(const ApiRequest &request)
{
    return createNetworkRequest(request);
}
