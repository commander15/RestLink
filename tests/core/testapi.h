#ifndef TESTAPI_H
#define TESTAPI_H

#include <RestLink/api.h>

using namespace RestLink;

class TestApi : public Api
{
    Q_OBJECT

public:
    explicit TestApi(QObject *parent = nullptr);

    QNetworkRequest generateNetworkRequest(const ApiRequest &request);
};

#endif // TESTAPI_H
