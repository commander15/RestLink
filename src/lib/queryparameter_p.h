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
};

}

#endif // RESTLINK_QUERYPARAMETER_P_H
