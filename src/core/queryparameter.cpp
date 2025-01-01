#include "queryparameter.h"
#include "queryparameter_p.h"

#include <QtCore/qvariant.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace RestLink {

QueryParameter::QueryParameter()
    : d_ptr(new QueryParameterData())
{
}

QueryParameter::QueryParameter(const QString &name, const QVariant &value)
    : d_ptr(new QueryParameterData())
{
    d_ptr->name = d_ptr->validateName(name);
    d_ptr->values = d_ptr->validateValues({value});
}

QueryParameter::QueryParameter(const QueryParameter &other) :
    d_ptr(other.d_ptr)
{
}

/*!
 * \brief Destructor for QueryParameter.
 *
 * Cleans up resources held by the QueryParameter object.
 */
QueryParameter::~QueryParameter()
{
}

QueryParameter &QueryParameter::operator=(const QueryParameter &other)
{
    if (this != &other)
        d_ptr = other.d_ptr;
    return *this;
}

QueryParameter &QueryParameter::operator=(QueryParameter &&other)
{
    if (this != &other)
        d_ptr = std::move(other.d_ptr);
    return *this;
}

/*!
 * \brief Retrieves the name of the parameter.
 * \return The parameter name as a QString.
 */
QString QueryParameter::name() const
{
    return d_ptr->name;
}

/*!
 * \brief Sets the name of the parameter.
 * \param name The name to set for the parameter.
 */
void QueryParameter::setName(const QString &name)
{
    d_ptr->name = d_ptr->validateName(name);
}

/*!
 * \brief Retrieves the value of the parameter.
 * \return The parameter value as a QVariant.
 */
QVariant QueryParameter::value() const
{
    return (!d_ptr->values.isEmpty() ? d_ptr->values : QVariant());
}

/*!
 * \brief Sets the value of the parameter.
 * \param value The value to set for the parameter.
 */
void QueryParameter::setValue(const QVariant &value)
{
    d_ptr->values = d_ptr->validateValues({value});
}

bool QueryParameter::hasValue(const QVariant &value) const
{
    return d_ptr->values.contains(value);
}

void QueryParameter::addValue(const QVariant &value)
{
    d_ptr->values.append(value);
}

void QueryParameter::removeValue(const QVariant &value)
{
    d_ptr->values.removeOne(value);
}

QVariantList QueryParameter::values() const
{
    return d_ptr->values;
}

void QueryParameter::setValues(const QVariantList &values)
{
    d_ptr->values = values;
}

/*!
 * \brief Checks if a specific flag is set for the parameter.
 * \param flag The ParameterFlag to check.
 * \return \c true if the flag is set, otherwise \c false.
 */
bool QueryParameter::hasFlag(ParameterFlag flag) const
{
    return d_ptr->flags.testFlag(flag);
}

/*!
 * \brief Retrieves all flags set for the parameter.
 * \return The flags as a ParameterFlags bitmask.
 */
QueryParameter::ParameterFlags QueryParameter::flags() const
{
    return d_ptr->flags;
}

/*!
 * \brief Sets or clears a specific flag for the parameter.
 * \param flag The ParameterFlag to modify.
 * \param on \c true to set the flag, \c false to clear it.
 */
void QueryParameter::setFlag(ParameterFlag flag, bool on)
{
    d_ptr->flags = d_ptr->validateFlags(flag);
}

/*!
 * \brief Sets multiple flags for the parameter.
 * \param flags The ParameterFlags bitmask to set.
 */
void QueryParameter::setFlags(const ParameterFlags &flags)
{
    d_ptr->flags = d_ptr->validateFlags(flags);
}

/*!
 * \brief Checks if the parameter is enabled.
 * \return \c true if the parameter is enabled, otherwise \c false.
 */
bool QueryParameter::isEnabled() const
{
    return d_ptr->enabled;
}

/*!
 * \brief Loads parameter information from a JSON object.
 * \param object The QJsonObject containing the parameter data.
 */
void QueryParameter::setEnabled(bool enabled)
{
    d_ptr->enabled = enabled;
}

/*!
 * \brief Checks if the parameter is valid.
 * \return \c true if the parameter is valid, otherwise \c false.
 *
 * A parameter is considered valid if it has a non-empty name and a not null value.
 */
bool QueryParameter::isValid() const
{
    return !d_ptr->name.isEmpty();
}

QJsonObject QueryParameter::toJsonObject() const
{
    QJsonObject object;
    object.insert("name", d_ptr->name);
    object.insert("values", QJsonValue::fromVariant(d_ptr->values));
    object.insert("enabled", d_ptr->enabled);

    if (d_ptr->flags.testFlag(AuthenticationFlag))
        object.insert("authentication", true);

    if (d_ptr->flags.testFlag(SecretFlag))
        object.insert("secret", true);

    return object;
}

/*!
 * \brief Loads parameter information from a JSON object.
 * \param object The QJsonObject containing the parameter data.
 */
QueryParameter QueryParameter::fromJsonbject(const QJsonObject &object)
{
    QueryParameter param;

    QueryParameterData *data = param.d_ptr;
    data->name = data->validateName(object.value("name").toString());
    data->values = data->validateValues({object.value("value").toVariant()});

    auto setFlag = [object, data](const QString &name, ParameterFlag flag, bool defaultValue) {
        if (object.contains(name))
            data->flags.setFlag(flag, object.value(name).toBool());
        else
            data->flags.setFlag(flag, defaultValue);
    };

    setFlag("authentication", AuthenticationFlag, false);
    setFlag("secret", SecretFlag, false);

    return param;
}

/*!
 * \brief Swaps the contents of this QueryParameter with another.
 * \param other The QueryParameter instance to swap with.
 */
void QueryParameter::swap(QueryParameter &other)
{
    d_ptr.swap(other.d_ptr);
}

/*!
 * \brief Merges two QueryParameter objects into a single parameter.
 * \param p1 The first QueryParameter to merge.
 * \param p2 The second QueryParameter to merge.
 * \return A new QueryParameter containing merged properties.
 */
QueryParameter QueryParameter::merge(const QueryParameter &r1, const QueryParameter &r2)
{
    // ToDo: rework
    return r1;
}

QString QueryParameterData::validateName(const QString &name)
{
    return name;
}

QVariantList QueryParameterData::validateValues(const QVariantList &values)
{
    return values;
}

QueryParameter::ParameterFlags QueryParameterData::validateFlags(QueryParameter::ParameterFlags flags)
{
    if (flags.testFlags(QueryParameter::AuthenticationFlag))
        flags.setFlag(QueryParameter::SecretFlag);
    return flags;
}

} // namespace RestLink
