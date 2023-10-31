#ifndef RESTLINK_APIREQUEST_P_H
#define RESTLINK_APIREQUEST_P_H

#include "apirequest.h"

namespace RestLink {

class ApiRequestPrivate : public QSharedData
{
public:
    ApiRequestPrivate();
    ApiRequestPrivate(const ApiRequestPrivate &other);
    ~ApiRequestPrivate();

    bool equals(const ApiRequestPrivate *other) const;

    QString endpoint;
    QVector<ApiRequestParameter> parameters;
    QByteArray data;
    ApiRequest::DataType dataType;
    ApiRequest::RequestVerb verb;
};

class ApiRequestParameterPrivate : public QSharedData
{
public:
    ApiRequestParameterPrivate();
    ApiRequestParameterPrivate(const ApiRequestParameterPrivate &other);
    ~ApiRequestParameterPrivate();

    bool equals(const ApiRequestParameterPrivate *other) const;

    QString name;
    QVariant value;
    ApiRequestParameter::ApiRequestParameterScope scope;
    bool enabled;
};

}

#endif // RESTLINK_APIREQUEST_P_H
