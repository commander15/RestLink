#ifndef RESTLINK_PATHPARAMETER_P_H
#define RESTLINK_PATHPARAMETER_P_H

#include <RestLink/private/parameter_p.h>

namespace RestLink {

class PathParameterData : public ParameterData
{
public:
    Parameter::Type type() const override
    { return Parameter::PathParameterType; }
};

}

#endif // RESTLINK_PATHPARAMETER_P_H
