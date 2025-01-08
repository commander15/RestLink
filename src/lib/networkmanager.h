#ifndef RESTLINK_NETWORKMANAGER_H
#define RESTLINK_NETWORKMANAGER_H

#include <RestLink/global.h>
#include <RestLink/api.h>

#include <QtNetwork/qnetworkaccessmanager.h>

namespace RestLink {

class RESTLINK_EXPORT NetworkManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);

    Response *head(const Request &request, Api *api);
    Response *get(const Request &request, Api *api);
    Response *post(const Request &request, const Body &body, Api *api);
    Response *put(const Request &request, const Body &body, Api *api);
    Response *patch(const Request &request, const Body &body, Api *api);
    Response *deleteResource(const Request &request, Api *api);

    Response *send(Api::Operation operation, const Request &request, const Body &body, Api *api);
    Response *send(Api::Operation operation, const Request &request, const Body &body, ApiBase *api);

    QStringList supportedSchemes() const;

protected:
    QNetworkRequest createNetworkRequest(Api::Operation operation, const Request &request, const Body &body, ApiBase *api);
    QNetworkReply *createNetworkReply(Api::Operation operation, const QNetworkRequest &request, const Body &body);

private:
    static void initHandlers();
    static QVector<class NetworkRequestHandler *> s_handlers;

    friend class Api;
    friend class NetworkRequestHandler;
};

class RESTLINK_EXPORT NetworkRequestHandler
{
public:
    virtual QStringList schemes() const = 0;

    virtual Response *send(Api::Operation operation, const Request &request, const Body &body, Api *api) = 0;

protected:
    QNetworkRequest createNetworkRequest(Api::Operation operation, const Request &request, const Body &body, ApiBase *api);

    NetworkManager *manager;

    friend class NetworkManager;
};

}

#endif // RESTLINK_NETWORKMANAGER_H
