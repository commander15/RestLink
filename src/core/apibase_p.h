#ifndef RESTLINK_APIBASE_P_H
#define RESTLINK_APIBASE_P_H

#include "apibase.h"

namespace RestLink {

class ApiReply;

class ApiBasePrivate
{
public:
    ApiBasePrivate(ApiBase *q);

    QNetworkAccessManager *netMan() const;
    void setNetMan(QNetworkAccessManager *man);

    static QByteArray httpVerbFromOperation(int op);

    ApiBase *q_ptr;

private:
    mutable QNetworkAccessManager *m_netMan;
};

}

#endif // RESTLINK_APIBASE_P_H
