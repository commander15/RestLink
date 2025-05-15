#ifndef RESTLINK_NETWORKMANAGER_H
#define RESTLINK_NETWORKMANAGER_H

#include <RestLink/global.h>
#include <RestLink/abstractrequesthandler.h>

#include <QtNetwork/qnetworkaccessmanager.h>

namespace RestLink {

class RESTLINK_EXPORT NetworkManager : public QNetworkAccessManager, public AbstractRequestHandler
{
    Q_OBJECT
    Q_PROPERTY(QStringList supportedSchemes READ supportedSchemes CONSTANT FINAL)

public:
    explicit NetworkManager(QObject *parent = nullptr);

    using AbstractRequestHandler::head;
    using AbstractRequestHandler::get;
    using AbstractRequestHandler::post;
    using AbstractRequestHandler::put;
    using AbstractRequestHandler::patch;
    using AbstractRequestHandler::deleteResource;

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
