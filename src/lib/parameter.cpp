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

/*!
 * \class RestLink::Parameter
 * \brief Represents a generic parameter, which could be a path, query, or header parameter, with support for special values and flags.
 *
 * This class provides a flexible structure to define parameters in various contexts, supporting operations
 * such as adding/removing values, flagging special attributes, and converting to/from JSON format.
 * Parameters can represent different types, including path parameters, query parameters, and headers.
 */

/*!
 * \enum Parameter::Flag
 * \brief Defines flags used to specify special attributes for a parameter.
 *
 * Flags can be used to indicate attributes like authentication, secrecy, and locale.
 *
 * \var Parameter::Flag Parameter::NoFlag
 * No flags are set. This is the default state.
 * \var Parameter::Flag Parameter::Authentication
 * Indicates that the parameter is used for authentication purposes.
 * \var Parameter::Flag Parameter::Secret
 * Indicates that the parameter holds secret or sensitive information.
 * \var Parameter::Flag Parameter::Locale
 * Indicates that the parameter is related to the locale or language settings.
 */

/*!
 * \enum Parameter::Type
 * \brief Defines the possible types of parameters.
 *
 * Parameters can be classified into different types, including base type, path parameter type, query parameter type,
 * and header type.
 *
 * \var Parameter::Type Parameter::BaseType
 * Represents the base type of the parameter, typically a general or default type.
 * \var Parameter::Type Parameter::PathParameterType
 * Represents parameters that are embedded in the URL path, usually used for resource identifiers.
 * \var Parameter::Type Parameter::QueryParameterType
 * Represents parameters that are passed as part of the query string in the URL.
 * \var Parameter::Type Parameter::HeaderType
 * Represents parameters that are used as HTTP headers.
 */

/*!
 * \brief Default constructor for Parameter class.
 *
 * Initializes a new parameter instance with default values.
 */
Parameter::Parameter()
    : d_ptr(new ParameterData())
{
}

/*!
 * \brief Copy constructor for Parameter class.
 *
 * Creates a new Parameter instance as a copy of the provided parameter.
 *
 * \param other The parameter to copy.
 */
Parameter::Parameter(const Parameter &other)
    : d_ptr(other.d_ptr)
{
}

/*!
 * \brief Move constructor for Parameter class.
 *
 * Transfers ownership of the resources from the provided parameter to the new instance.
 *
 * \param other The parameter to move.
 */
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

/*!
 * \brief Destructor for Parameter class.
 *
 * Cleans up the resources held by the parameter.
 */
Parameter::~Parameter()
{
}

/*!
 * \brief Copy assignment operator for Parameter class.
 *
 * Copies the contents of one Parameter instance to another.
 *
 * \param other The parameter to copy from.
 * \return A reference to the current instance.
 */
Parameter &Parameter::operator=(const Parameter &other)
{
    if (this != &other)
        d_ptr = other.d_ptr;
    return *this;
}

/*!
 * \brief Move assignment operator for Parameter class.
 *
 * Transfers ownership of the resources from the provided parameter to the current instance.
 *
 * \param other The parameter to move from.
 * \return A reference to the current instance.
 */
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

/*!
 * \brief Returns a special value for the parameter, based on the provided API.
 *
 * This method may return a specific value depending on the context of the API.
 *
 * \param api The API used to determine the special value.
 * \return A QVariant representing the special value.
 */
QVariant Parameter::specialValue(Api *api) const
{
    // ToDo: Optimize for better performances
    const QVariantList values = specialValues(api);
    return !values.isEmpty() ? values.first() : QVariant();
}

/*!
 * \brief Returns a list of special values for the parameter, based on the provided API.
 *
 * This method may return multiple special values depending on the context of the API.
 *
 * \param api The API used to determine the special values.
 * \return A list of special values as QVariantList.
 */
QVariantList Parameter::specialValues(Api *api) const
{
    QVariantList values;

    const QVariant mainValue = this->value();
    std::transform(d_ptr->values.begin(), d_ptr->values.end(), std::back_inserter(values), [this, api, &mainValue](const QVariant &value) {
        if (hasFlag(Locale)) {
            const QLocale locale = api->locale();
            const QString format = mainValue.toString();

            if (format == ".")
                return QVariant(locale.name().section("_", 0, 0));
            else if (format == "-")
                return QVariant(locale.name(QLocale::TagSeparator::Dash));
            else
                return QVariant(locale.name(QLocale::TagSeparator::Underscore));
        }

        return value;
    });

    return values;
}

/*!
 * \brief Checks if the parameter contains a specific value.
 *
 * \param value The value to check.
 * \return True if the value exists in the parameter, false otherwise.
 */
bool Parameter::hasValue(const QVariant &value) const
{
    return d_ptr->values.contains(value);
}

/*!
 * \brief Adds a value to the parameter.
 *
 * \param value The value to add.
 */
void Parameter::addValue(const QVariant &value)
{
    const QVariant finalValue = d_ptr->validateValue(value);
    if (finalValue.isValid())
        d_ptr->values.append(finalValue);
}

/*!
 * \brief Removes a value from the parameter.
 *
 * \param value The value to remove.
 */
void Parameter::removeValue(const QVariant &value)
{
    d_ptr->values.removeOne(value);
}

/*!
 * \brief Returns a list of all values associated with the parameter.
 *
 * \return A list of values as QList<QVariant>.
 */
QVariantList Parameter::values() const
{
    return d_ptr->values;
}

/*!
 * \brief Sets a list of values for the parameter.
 *
 * \param values The new list of values to assign.
 */
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

/*!
 * \brief Returns the type of the parameter.
 *
 * \return The parameter's type.
 */
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

/*!
 * \brief Converts the parameter to a QJsonObject.
 *
 * \return The parameter represented as a QJsonObject.
 */
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

/*!
 * \brief Equality operator for Parameter class.
 *
 * Two parameters are considered equal if they have the same name, value, and flags.
 *
 * \param other The parameter to compare with.
 * \return True if the parameters are equal, false otherwise.
 */
bool Parameter::operator==(const Parameter &other) const
{
    return d_ptr->name == other.d_ptr->name
        && d_ptr->values == other.d_ptr->values
        && d_ptr->flags == other.d_ptr->flags;
}

/*!
 * \brief Inequality operator for Parameter class.
 *
 * Two parameters are considered unequal if they differ in name, value, or flags.
 *
 * \param other The parameter to compare with.
 * \return True if the parameters are unequal, false otherwise.
 */

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
