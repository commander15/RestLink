#include "pathparameter.h"

#include <RestLink/private/parameter_p.h>

namespace RestLink {

class PathParameterData : public ParameterData
{
public:
    Parameter::Type type() const override
    { return Parameter::PathParameter; }
};

PathParameter::PathParameter()
    : Parameter(new PathParameterData())
{
}

PathParameter::PathParameter(const QString &name, const QVariant &value)
    : Parameter(new PathParameterData)
{
    setName(name);
    setValue(value);
}

PathParameter &PathParameter::operator=(const PathParameter &other)
{
    Parameter::operator=(other);
    return *this;
}

PathParameter PathParameter::fromJsonObject(const QJsonObject &object)
{
    PathParameter p;
    dataFromJsonObject(p.d_ptr, object);
    return p;
}

PathParameter &PathParameter::operator=(PathParameter &&other)
{
    Parameter::operator=(std::move(other));
    return *this;
}

bool PathParameter::operator==(const PathParameter &other)
{
    return Parameter::operator==(other);
}

} // namespace RestLink
