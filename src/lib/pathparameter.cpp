#include "pathparameter.h"

namespace RestLink {

PathParameter::PathParameter()
{
}

PathParameter::PathParameter(const QString &name, const QVariant &value)
    : m_name(name)
    , m_value(value)
{
}

PathParameter &PathParameter::operator=(const PathParameter &other)
{
    if (this != &other) {
        m_name = other.m_name;
        m_value = other.m_value;
    }
    return *this;
}

PathParameter &PathParameter::operator=(PathParameter &&other)
{
    if (this != &other) {
        m_name = std::move(other.m_name);
        m_value = std::move(other.m_value);
    }
    return *this;
}

QString PathParameter::name() const
{
    return m_name;
}

void PathParameter::setName(const QString &name)
{
    m_name = name;
}

QVariant PathParameter::value() const
{
    return m_value;
}

void PathParameter::setValue(const QVariant &value)
{
    m_value = value;
}

bool PathParameter::isValid() const
{
    return !m_name.isEmpty() && m_value.isValid();
}

bool PathParameter::operator==(const PathParameter &other)
{
    return m_name == other.m_name && m_value == other.m_value;
}

} // namespace RestLink
