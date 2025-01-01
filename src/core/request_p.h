#ifndef RESTLINK_REQUEST_P_H
#define RESTLINK_REQUEST_P_H

#include <RestLink/queryparameter.h>
#include <RestLink/pathparameter.h>
#include <RestLink/header.h>

#include <QtCore/qlist.h>

namespace RestLink {

class RequestData : public QSharedData
{
public:
    static QString validateEndpoint(const QString &input);

    QList<PathParameter>::const_iterator findPathParameter(const QString &name) const;
    QList<PathParameter>::iterator findPathParameter(const QString &name);

    QList<QueryParameter>::const_iterator findQueryParameter(const QString &name) const;
    QList<QueryParameter>::iterator findQueryParameter(const QString &name);

    QList<Header>::const_iterator findHeader(const QString &name) const;
    QList<Header>::iterator findHeader(const QString &name);

    QString endpoint;
    QList<PathParameter> pathParameters;
    QList<QueryParameter> queryParameters;
    QList<Header> headers;
};

}

#endif // RESTLINK_REQUEST_P_H
