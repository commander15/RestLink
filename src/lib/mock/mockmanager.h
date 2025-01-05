#ifndef RESTLINK_MOCKMANAGER_H
#define RESTLINK_MOCKMANAGER_H

#include <RestLink/networkmanager.h>
#include <RestLink/header.h>
#include <RestLink/body.h>

namespace RestLink {

class MockData;

class RESTLINK_EXPORT MockManager : public RestLink::NetworkManager
{
    Q_OBJECT

public:
    explicit MockManager(QObject *parent = nullptr);

protected:
    QNetworkReply *createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData) override;

private:
    MockData findMockData(const QNetworkRequest &request) const;
};

class RESTLINK_EXPORT MockData
{
public:
    bool isValid() const
    { return !headers.isEmpty() || (body.isData() || body.isDevice()); }

    QList<Header> headers;
    Body body;
};

} // namespace RestLink

#endif // RESTLINK_MOCKMANAGER_H
