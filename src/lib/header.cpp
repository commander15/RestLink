#include "header.h"

#include <RestLink/private/parameter_p.h>

namespace RestLink {

class HeaderData : public ParameterData
{
public:
    Parameter::Type type() const override
    { return Parameter::Header; }
};

Header::Header()
    : Parameter(new HeaderData())
{
}

Header::Header(const QString &name, const QVariant &value)
    : Parameter(new HeaderData())
{
    setName(name);
    setValue(value);
}

Header &Header::operator=(const Header &other)
{
    Parameter::operator=(other);
    return *this;
}

Header Header::fromJsonObject(const QJsonObject &object)
{
    Header h;
    dataFromJsonObject(h.d_ptr, object);
    return h;
}

}
