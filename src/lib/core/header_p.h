#ifndef RESTLINK_HEADER_P_H
#define RESTLINK_HEADER_P_H

#include <RestLink/private/parameter_p.h>

namespace RestLink {

class HeaderData : public ParameterData
{
public:
    Parameter::Type type() const override
    { return Parameter::HeaderType; }
};

}

#endif // RESTLINK_HEADER_P_H
