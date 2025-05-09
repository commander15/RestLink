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
    Q_PROPERTY(QStringList supportedSchemes READ supportedSchemes CONSTANT FINAL)

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
    Response *sendRequest(Method method, const Request &request, const Body &body) override;

    QNetworkRequest generateNetworkRequest(Method method, const Request &request, const Body &body);
    QNetworkReply *generateNetworkReply(Method method, const QNetworkRequest &request, const Body &body);

    friend class Api;
};

}

#endif // RESTLINK_NETWORKMANAGER_H
