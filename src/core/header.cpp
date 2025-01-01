#include "header.h"

#include <QtCore/qjsonobject.h>

namespace RestLink {

Header::Header() :
    m_enabled(true)
{
}

Header::Header(const QString &name, const QVariant &value) :
    m_name(name),
    m_values({value}),
    m_enabled(true)
{
}

Header &Header::operator=(const Header &other)
{
    if (this != &other) {
        m_name = other.m_name;
        m_values = other.m_values;
        m_enabled = other.m_enabled;
    }
    return *this;
}

QString Header::name() const
{
    return m_name;
}

void Header::setName(const QString &name)
{
    m_name = name;
}

QVariant Header::value() const
{
    return (!m_values.isEmpty() ? m_values.first() : QVariant());
}

void Header::setValue(const QVariant &value)
{
    if (!m_values.isEmpty())
        m_values.replace(0, value);
    else
        m_values.append(value);
}

bool Header::hasValue(const QVariant &value) const
{
    return m_values.contains(value);
}

void Header::addValue(const QVariant &value)
{
    m_values.append(value);
}

void Header::removeValue(const QVariant &value)
{
    m_values.removeOne(value);
}

QVariantList Header::values() const
{
    return m_values;
}

void Header::setValues(const QVariantList &values)
{
    m_values = values;
}

bool Header::isEnabled() const
{
    return m_enabled;
}

void Header::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

Header Header::fromJsonObject(const QJsonObject &object)
{
    Header header;
    header.m_name = object.value("name").toString();
    if (object.contains("value"))
        header.m_values = { object.value("value").toVariant() };
    // ToDo: handle multiple values
    return header;
}

QJsonObject Header::toJsonObject() const
{
    QJsonObject object;
    object.insert("name", m_name);
    if (m_values.size() == 1)
        object.insert("value", QJsonValue::fromVariant(m_values.first()));
    // ToDo: handle multiple values
    return object;
}

}
