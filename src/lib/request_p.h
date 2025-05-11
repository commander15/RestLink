#ifndef RESTLINK_REQUEST_P_H
#define RESTLINK_REQUEST_P_H

#include "request.h"

#include <RestLink/queryparameter.h>
#include <RestLink/pathparameter.h>
#include <RestLink/header.h>

#include <QtCore/qurl.h>
#include <QtCore/qlist.h>

namespace RestLink {

class RequestPrivate : public QSharedData
{
public:
    RequestPrivate() = default;
    RequestPrivate(const RequestPrivate &other) = default;
    virtual ~RequestPrivate() = default;

    virtual RequestPrivate *clone() const
    { return new RequestPrivate(*this); }

    static QString validateEndpoint(const QString &input);

    static bool canUseUrlParameter(const Parameter &parameter, Request::UrlType type);

    QString endpoint;
    QUrl baseUrl;
    QList<PathParameter> pathParameters;
    QList<QueryParameter> queryParameters;
    QList<Header> headers;
    QHash<Request::Attribute, QVariant> attributes;

    RequestProcessing processing;
    AbstractController *controller = nullptr;

    Api *api = nullptr;
};

}

#endif // RESTLINK_REQUEST_P_H
