#include "mockmanager.h"

#include "mockreply.h"

namespace RestLink {

MockManager::MockManager(QObject *parent)
    : RestLink::NetworkManager{parent}
{
}

QNetworkReply *MockManager::createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    const MockData mockData = findMockData(request);
    if (mockData.isValid()) {
        return MockReply::create(request, mockData, this);
    } else {
        return NetworkManager::createRequest(op, request, outgoingData);
    }
}

MockData MockManager::findMockData(const QNetworkRequest &request) const
{
    return MockData();
}

} // namespace RestLink
