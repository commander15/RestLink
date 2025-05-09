#ifndef RESTLINK_QUERYPARAMETER_P_H
#define RESTLINK_QUERYPARAMETER_P_H

#include <RestLink/private/parameter_p.h>

#include <QtCore/qvariant.h>

namespace RestLink {

class QueryParameterData : public ParameterData
{
public:
    Parameter::Type type() const override
    { return Parameter::QueryParameterType; }

    // Query must have at least one value
    QVariant validateValue(const QVariant &value) const override
    { return value.isNull() ? QString() : value; }
};

}

#endif // RESTLINK_QUERYPARAMETER_P_H
