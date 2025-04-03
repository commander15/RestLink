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

    using RequestHandler::head;
    using RequestHandler::get;
    using RequestHandler::post;
    using RequestHandler::put;
    using RequestHandler::patch;
    using RequestHandler::deleteResource;

    QStringList supportedSchemes() const override final;
    HandlerType handlerType() const override final;

protected:
    Response *sendRequest(ApiBase::Operation operation, const Request &request, const Body &body) override;

    QNetworkRequest generateNetworkRequest(Api::Operation operation, const Request &request, const Body &body);
    QNetworkReply *generateNetworkReply(Api::Operation operation, const QNetworkRequest &request, const Body &body);

    friend class Api;
};

}

#endif // RESTLINK_NETWORKMANAGER_H
