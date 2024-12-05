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

    bool cache;
};

class ApiRequestParameterPrivate : public QSharedData
{
public:
    ApiRequestParameterPrivate();
    ApiRequestParameterPrivate(const ApiRequestParameterPrivate &other);
    ~ApiRequestParameterPrivate();

    bool equals(const ApiRequestParameterPrivate *other) const;

    void correctFlags();
    void adaptToFlag(ApiRequestParameter::ParameterFlag flag);

    //static QString scopeName(ApiRequestParameter::ApiRequestParameterScope scope);
    static ApiRequestParameter::ParameterScope nameScope(const QString &name);

    QString name;
    QVariant value;
    ApiRequestParameter::ParameterScope scope;
    ApiRequestParameter::ParameterFlags flags;
    bool enabled;
};

}

#endif // RESTLINK_APIREQUEST_P_H
