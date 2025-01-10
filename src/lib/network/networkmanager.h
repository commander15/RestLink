#ifndef RESTLINK_NETWORKMANAGER_H
#define RESTLINK_NETWORKMANAGER_H

#include <RestLink/global.h>
#include <RestLink/requesthandler.h>
#include <RestLink/api.h>

#include <QtNetwork/qnetworkaccessmanager.h>

namespace RestLink {

class RESTLINK_EXPORT NetworkManager : public QNetworkAccessManager, public RequestHandler
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);

    QStringList supportedSchemes() const override;

protected:
    Response *sendRequest(ApiBase::Operation operation, const Request &request, const Body &body, Api *api) override;

    QNetworkRequest generateNetworkRequest(Api::Operation operation, const Request &request, const Body &body, ApiBase *api);
    QNetworkReply *generateNetworkReply(Api::Operation operation, const QNetworkRequest &request, const Body &body, ApiBase *api);

private:
    QVector<RequestHandler *> m_handlers;

    friend class Api;
};

}

#endif // RESTLINK_NETWORKMANAGER_H
