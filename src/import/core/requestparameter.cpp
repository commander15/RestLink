#include "requestparameter.h"

#include <RestLink/pathparameter.h>
#include <RestLink/queryparameter.h>
#include <RestLink/header.h>

namespace RestLink {
namespace Qml {

RequestParameter::RequestParameter(QObject *parent)
    : QObject(parent)
    , m_type(Parameter::QueryParameterType)
    , m_enabled(true)
{
}

Parameter RequestParameter::parameter() const
{
    Parameter p;

    switch (m_type) {
    case Parameter::PathParameterType:
        p = RestLink::PathParameter(m_name, m_value);
        break;

    case Parameter::QueryParameterType:
        p = RestLink::QueryParameter(m_name, m_value);
        break;

    case Parameter::HeaderType:
        p = RestLink::Header(m_name, m_value);
        break;

    default:
        break;
    }

    auto setFlag = [this, &p](const char *property, Parameter::Flag flag) {
        bool on = this->property(property).toBool();
        if (on)
            p.setFlag(flag, true);
    };

    setFlag("authentication", Parameter::Authentication);
    setFlag("secret", Parameter::Secret);
    setFlag("locale", Parameter::Locale);

    return p;
}

bool RequestParameter::isEnabled() const
{
    return m_enabled;
}

} // namespace Qml
} // namespace RestLink
