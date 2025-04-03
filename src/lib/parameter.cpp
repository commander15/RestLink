#include "parameter.h"
#include "parameter_p.h"

#include <RestLink/api.h>
#include <RestLink/pathparameter.h>
#include <RestLink/queryparameter.h>
#include <RestLink/header.h>

#include <RestLink/private/pathparameter_p.h>
#include <RestLink/private/queryparameter_p.h>
#include <RestLink/private/header_p.h>

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace RestLink {

Parameter::Parameter()
    : d_ptr(new ParameterData())
{
}

Parameter::Parameter(const Parameter &other)
    : d_ptr(other.d_ptr)
{
}

Parameter::Parameter(Parameter &&other)
    : d_ptr(std::move(other.d_ptr))
{
}

Parameter::Parameter(ParameterData *d)
    : d_ptr(d)
{
}

Parameter::Parameter(const QSharedDataPointer<ParameterData> &data)
    : d_ptr(data)
{
}

Parameter::~Parameter()
{
}

Parameter &Parameter::operator=(const Parameter &other)
{
    if (this != &other)
        d_ptr = other.d_ptr;
    return *this;
}

Parameter &Parameter::operator=(Parameter &&other)
{
    if (this != &other)
        d_ptr.swap(other.d_ptr);
    return *this;
}

/*!
 * \brief Retrieves the name of the parameter.
 * \return The parameter name as a QString.
 */
QString Parameter::name() const
{
    return d_ptr->name;
}

/*!
 * \brief Sets the name of the parameter.
 * \param name The name to set for the parameter.
 */
void Parameter::setName(const QString &name)
{
    d_ptr->name = d_ptr->validateName(name);
}

/*!
 * \brief Retrieves the value of the parameter.
 * \return The parameter value as a QVariant.
 */
QVariant Parameter::value() const
{
    return (!d_ptr->values.isEmpty() ? d_ptr->values.constFirst() : QVariant());
}

/*!
 * \brief Sets the value of the parameter.
 * \param value The value to set for the parameter.
 */
void Parameter::setValue(const QVariant &value)
{
    const QVariant finalValue = d_ptr->validateValue(value);
    if (finalValue.isValid())
        d_ptr->values = { finalValue };
    else
        d_ptr->values.clear();
}

QVariant Parameter::specialValue(Api *api) const
{
    // ToDo: Optimize for better performances
    const QVariantList values = specialValues(api);
    return !values.isEmpty() ? values.first() : QVariant();
}

QVariantList Parameter::specialValues(Api *api) const
{
    QVariantList values;

    std::transform(d_ptr->values.begin(), d_ptr->values.end(), std::back_inserter(values), [this, api](const QVariant &value) {
        // ToDo: wrap in a private method for reusability
        if (!d_ptr->values.isEmpty())
            return d_ptr->values.first();

        if (hasFlag(Locale)) {
            const QString full = api->locale().name();
            //const QString slim = full.section('_', 0, 0);
            return QVariant(full);
        }

        return value;
    });

    return values;
}

bool Parameter::hasValue(const QVariant &value) const
{
    return d_ptr->values.contains(value);
}

void Parameter::addValue(const QVariant &value)
{
    const QVariant finalValue = d_ptr->validateValue(value);
    if (finalValue.isValid())
        d_ptr->values.append(finalValue);
}

void Parameter::removeValue(const QVariant &value)
{
    d_ptr->values.removeOne(value);
}

QVariantList Parameter::values() const
{
    return d_ptr->values;
}

void Parameter::setValues(const QVariantList &values)
{
    d_ptr->values = d_ptr->validateValues(values);
}

/*!
 * \brief Checks if a specific flag is set for the parameter.
 * \param flag The ParameterFlag to check.
 * \return \c true if the flag is set, otherwise \c false.
 */
bool Parameter::hasFlag(Flag flag) const
{
    return d_ptr->flags.testFlag(flag);
}

/*!
 * \brief Retrieves all flags set for the parameter.
 * \return The flags as a ParameterFlags bitmask.
 */
Parameter::Flags Parameter::flags() const
{
    return d_ptr->flags;
}

/*!
 * \brief Sets or clears a specific flag for the parameter.
 * \param flag The ParameterFlag to modify.
 * \param on \c true to set the flag, \c false to clear it.
 */
void Parameter::setFlag(Flag flag, bool on)
{
    d_ptr->flags |= d_ptr->validateFlags(flag);
}

/*!
 * \brief Sets multiple flags for the parameter.
 * \param flags The ParameterFlags bitmask to set.
 */
void Parameter::setFlags(const Flags &flags)
{
    d_ptr->flags = d_ptr->validateFlags(flags);
}

Parameter::Type Parameter::type() const
{
    return d_ptr->type();
}

/*!
 * \brief Checks if the parameter is valid.
 * \return \c true if the parameter is valid, otherwise \c false.
 *
 * A parameter is considered valid if it has a non-empty name and a not null value.
 */
bool Parameter::isValid() const
{
    return !d_ptr->isValid();
}

PathParameter Parameter::toPathParameter() const
{
    if (d_ptr->type() == PathParameterType)
        return PathParameter(d_ptr);
    else
        return PathParameter();
}

/**
 * Converts the current Parameter instance to a QueryParameter.
 * If the type of the parameter is QueryParameterType, it returns a valid QueryParameter.
 * Otherwise, it returns an invalid QueryParameter instance.
 *
 * @return QueryParameter - A converted QueryParameter instance or an invalid one.
 */
QueryParameter Parameter::toQueryParameter() const
{
    if (d_ptr->type() == QueryParameterType)
        return QueryParameter(d_ptr);
    else
        return QueryParameter();
}

/**
 * Converts the current Parameter instance to a Header.
 * If the type of the parameter is HeaderType, it returns a valid Header.
 * Otherwise, it returns an invalid Header instance.
 *
 * @return Header - A converted Header instance or an invalid one.
 */
Header Parameter::toHeader() const
{
    if (d_ptr->type() == HeaderType)
        return Header(d_ptr);
    else
        return Header();
}

QJsonObject Parameter::toJsonObject() const
{
    QJsonObject object;
    object.insert("name", d_ptr->name);

    if (!d_ptr->values.isEmpty()) {
        object.insert("value", QJsonValue::fromVariant(d_ptr->values.first()));

        QJsonArray values;
        for (const QVariant &value : d_ptr->values)
            values.append(QJsonValue::fromVariant(value));
        object.insert("values", values);
    }

    if (d_ptr->flags.testFlag(Authentication))
        object.insert("authentication", true);

    if (d_ptr->flags.testFlag(Secret))
        object.insert("secret", true);

    return object;
}

/*!
 * \brief Loads parameter information from a JSON object.
 * \param object The QJsonObject containing the parameter data.
 */
Parameter Parameter::fromJsonObject(const QJsonObject &object, Type type)
{
    ParameterData *data = new ParameterData();
    dataFromJsonObject(data, object);
    return Parameter(data);
}

/*!
 * \brief Merges two Parameter objects into a single parameter.
 * \param p1 The first Parameter to merge.
 * \param p2 The second Parameter to merge.
 * \return A new Parameter containing merged properties.
 */
Parameter Parameter::merge(const Parameter &r1, const Parameter &r2)
{
    // ToDo: rework
    return r1;
}

bool Parameter::operator==(const Parameter &other) const
{
    return d_ptr->name == other.d_ptr->name
        && d_ptr->values == other.d_ptr->values
        && d_ptr->flags == other.d_ptr->flags;
}

bool Parameter::operator!=(const Parameter &other) const
{
    return !operator==(other);
}

void Parameter::dataFromJsonObject(ParameterData *data, const QJsonObject &object)
{
    data->name = data->validateName(object.value("name").toString());
    data->values = data->validateValues({object.value("value").toVariant()});

    auto setFlag = [object, data](const QString &name, Flag flag, bool defaultValue) {
        bool value = (object.contains(name) ? object.value(name).toBool() : defaultValue);
        if (value)
            data->flags |= data->validateFlags(flag);
        else
            data->flags &= flag;
    };

    setFlag("authentication", Authentication, false);
    setFlag("secret", Secret, false);
    setFlag("locale", Locale, false);
}

} // namespace RestLink
