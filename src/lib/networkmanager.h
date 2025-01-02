#ifndef RESTLINK_NETWORKMANAGER_H
#define RESTLINK_NETWORKMANAGER_H

#include <RestLink/global.h>

#include <QtNetwork/qnetworkaccessmanager.h>

namespace RestLink {

class RESTLINK_EXPORT NetworkManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);
};

}

#endif // RESTLINK_NETWORKMANAGER_H
