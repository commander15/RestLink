#include "apirequest.h"
#include "apirequest_p.h"

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace RestLink {

/*!
 * \class ApiRequest
 * \brief A class for storing and managing HTTP request information.
 *
 * The ApiRequest class encapsulates the essential details of an HTTP request,
 * such as the endpoint, HTTP method, headers, query parameters, and more.
 * It provides a convenient way to define and organize API requests before
 * execution using RestLink.
 */

/*!
 * \brief Default constructor for ApiRequest.
 *
 * Initializes an empty ApiRequest object. This constructor sets up the
 * object with default values for its members, ensuring a consistent state.
 */
ApiRequest::ApiRequest() :
    d(new ApiRequestPrivate)
{
}

/*!
 * \brief Copy constructor for ApiRequest.
 *
 * Creates a new ApiRequest object as a copy of another. All data, including
 * headers, URL, and parameters, are duplicated into the new instance.
 *
 * \param other The ApiRequest object to copy.
 */
ApiRequest::ApiRequest(const ApiRequest &other) :
    d(other.d)
{
}

/*!
 * \brief Move constructor for ApiRequest.
 *
 * Transfers ownership of the data from another ApiRequest object to this one.
 * The source object is left in a valid but unspecified state.
 *
 * \param other The ApiRequest object to move from.
 */
ApiRequest::ApiRequest(ApiRequest &&other) :
    d(other.d)
{
}

/*!
 * \brief Destructor for ApiRequest.
 *
 * Cleans up any resources used by the ApiRequest object. Ensures all dynamically
 * allocated memory is released.
 */
ApiRequest::~ApiRequest()
{
}

/*!
 * \brief Copy assignment operator for ApiRequest.
 *
 * Copies the content of another ApiRequest object to this one, replacing the
 * existing data. Properly handles self-assignment.
 *
 * \param other The ApiRequest object to copy.
 * \return A reference to the updated ApiRequest object.
 */
ApiRequest &ApiRequest::operator=(const ApiRequest &other)
{
    if (this != &other)
        d = other.d;
    return *this;
}

/*!
 * \brief Move assignment operator for ApiRequest.
 *
 * Transfers ownership of the data from another ApiRequest object to this one.
 * The existing data in this object is replaced, and the source object is left
 * in a valid but unspecified state.
 *
 * \param other The ApiRequest object to move from.
 * \return A reference to the updated ApiRequest object.
 */
ApiRequest &ApiRequest::operator=(ApiRequest &&other)
{
    if (this != &other)
        d.swap(other.d);
    return *this;
}

/*!
 * \brief Retrieves the API endpoint associated with this request.
 * \return The endpoint as a QString.
 */
QString ApiRequest::endpoint() const
{
    return d->endpoint;
}

/*!
 * \brief Retrieves the API endpoint associated with this request.
 * \return The endpoint as a QString.
 */
void ApiRequest::setEndpoint(const QString &endpoint)
{
    if (endpoint.isEmpty())
        d->endpoint.clear();
    else if (!endpoint.startsWith('/'))
        d->endpoint = '/' + endpoint;
    else
        d->endpoint = endpoint;
}

/*!
 * \brief Checks if a parameter with the given name exists.
 * \param name The name of the parameter to check.
 * \return \c true if the parameter exists, otherwise \c false.
 */
bool ApiRequest::hasParameter(const QString &name) const
{
    for (const ApiRequestParameter &parameter : d->parameters)
        if (parameter.name() == name)
            return true;
    return false;
}

/*!
 * \brief Retrieves a parameter by its name.
 * \param name The name of the parameter to retrieve.
 * \return The corresponding ApiRequestParameter object.
 */
ApiRequestParameter ApiRequest::parameter(const QString &name) const
{
    for (const ApiRequestParameter &parameter : d->parameters)
        if (parameter.name() == name)
            return parameter;
    return ApiRequestParameter();
}

/*!
 * \brief Retrieves a list of all parameter names in the request.
 * \return A QStringList containing the names of all parameters.
 */
QStringList ApiRequest::parameterList() const
{
    QStringList parameters;
    for (const ApiRequestParameter &parameter : d->parameters)
        parameters.append(parameter.name());
    return parameters;
}

/*!
 * \brief Retrieves all parameters associated with the request.
 * \return A QList of ApiRequestParameter objects.
 */
QList<ApiRequestParameter> ApiRequest::parameters() const
{
    return d->parameters.toList();
}

/*!
 * \brief Adds a parameter to the request.
 * \param parameter The ApiRequestParameter object to add.
 */
void ApiRequest::addParameter(const ApiRequestParameter &parameter)
{
    if (!parameter.isValid())
        return;

    auto it = std::find_if(d->parameters.begin(), d->parameters.end(), [&parameter](const ApiRequestParameter &p) {
        return p.name() == parameter.name();
    });

    if (it == d->parameters.constEnd())
        d->parameters.append(parameter);
    else
        *it = parameter;
}

/*!
 * \brief Removes a parameter by its name.
 * \param name The name of the parameter to remove.
 */
void ApiRequest::removeParameter(const QString &name)
{
    d->parameters.removeIf([&name](const ApiRequestParameter &p) {
        return p.name() == name;
    });
}

/*!
 * \brief Sets the list of parameters for the request.
 * \param parameters The QList of ApiRequestParameter objects to set.
 */
void ApiRequest::setParameters(const QList<ApiRequestParameter> &parameters)
{
    d->parameters = parameters.toVector();
}

/*!
 * \brief Retrieves the content type of the request.
 * \return The content type as a QString.
 */
QString ApiRequest::contentType() const
{
    return d->contentType;
}

/*!
 * \brief Sets the content type for the request.
 * \param type The content type to set.
 */
void ApiRequest::setContentType(const QString &type)
{
    d->contentType = type;
}

/*!
 * \brief Checks if the request is marked as cacheable.
 * \return \c true if the request is cacheable, otherwise \c false.
 */
bool ApiRequest::isCacheable() const
{
    return d->cache;
}

/*!
 * \brief Sets whether the request is cacheable.
 * \param cache \c true to mark the request as cacheable, \c false otherwise.
 */
void ApiRequest::setCacheable(bool cache)
{
    d->cache = cache;
}

/*!
 * \brief Constructs the URL path for the request based on its parameters and endpoint.
 * \return The constructed URL path as a QString.
 */
QString ApiRequest::urlPath() const
{
    QStringList paths = d->endpoint.split('/');
    for (QString &path : paths)
        for (const ApiRequestParameter &param : d->parameters)
            if (param.scope() == ApiRequestParameter::UrlPathScope && path == '{' + param.name() + '}')
                path = param.value().toString();
    return paths.join('/');
}

/*!
 * \brief Loads request information from a JSON object.
 * \param object The QJsonObject containing the request data.
 * \param data Optional raw data associated with the request.
 */
void ApiRequest::loadFromJsonObject(const QJsonObject &object, QByteArray *data)
{
    d->endpoint = object.value("endpoint").toString();

    QJsonArray parameters = object.value("parameters").toArray();
    d->parameters.clear();
    for (const QJsonValue &parameterValue : parameters) {
        ApiRequestParameter requestParameter;
        requestParameter.loadFromJsonObject(parameterValue.toObject());
        d->parameters.append(requestParameter);
    }

    if (data)
        *data = object.value("data").toString().toUtf8();
}

/*!
 * \brief Swaps the contents of this ApiRequest with another.
 * \param other The ApiRequest instance to swap with.
 */
void ApiRequest::swap(ApiRequest &other)
{
    d.swap(other.d);
}

/*!
 * \brief Compares two ApiRequest objects for equality.
 * \param other The ApiRequest instance to compare with.
 * \return \c true if both requests are equal, otherwise \c false.
 */
bool ApiRequest::operator==(const ApiRequest &other) const
{
    if (this == &other)
        return true;
    else
        return d->equals(other.d);
}

/*!
 * \brief Merges two ApiRequest objects into a single request.
 * \param r1 The first ApiRequest to merge.
 * \param r2 The second ApiRequest to merge.
 * \return A new ApiRequest containing merged properties and parameters.
 */
ApiRequest ApiRequest::mergeRequests(const ApiRequest &r1, const ApiRequest &r2)
{
    ApiRequest r = r1;

    // add parameters of r2 while keeping r1 parameters, r2 override r1 parameters whenever possible
    const QList<ApiRequestParameter> parameters = r2.parameters();
    for (const ApiRequestParameter &parameter : parameters) {
        if (!r.hasParameter(parameter.name()))
            r.addParameter(parameter);
        else
            r.addParameter(ApiRequestParameter::mergeParameters(r.parameter(parameter.name()), parameter));
    }

    return r;
}

ApiRequestPrivate::ApiRequestPrivate() :
    cache(true)
{
}

ApiRequestPrivate::ApiRequestPrivate(const ApiRequestPrivate &other) :
    QSharedData(other),
    endpoint(other.endpoint),
    parameters(other.parameters),
    cache(other.cache)
{
}

ApiRequestPrivate::~ApiRequestPrivate()
{
}

bool ApiRequestPrivate::equals(const ApiRequestPrivate *other) const
{
    return endpoint == other->endpoint
            && parameters == other->parameters
            && cache == other->cache;
}

/*!
 * \class RestLink::ApiRequestParameter
 * \brief Represents a parameter in an API request.
 *
 * The ApiRequestParameter class is used to define and manage parameters that are included in
 * API requests. These parameters can be used in the URL path, query, or headers and may have
 * flags to indicate special handling (such as authentication). The parameter may also be enabled
 * or disabled based on specific requirements.
 *
 * \property QString name
 * \brief The name of the parameter.
 *
 * \property QVariant value
 * \brief The value of the parameter.
 *
 * \property ParameterScope scope
 * \brief The scope of the parameter, which determines where it should be included (URL path,
 *        query, or header).
 *
 * \property ParameterFlags flags
 * \brief The flags associated with the parameter that define special behaviors (e.g.,
 *        authentication or secret flags).
 *
 * \property bool enabled
 * \brief Whether the parameter is enabled and should be included in the API request.
 *
 * The class provides functionality to:
 * - Set and retrieve parameter names and values.
 * - Specify the scope (URL path, query, header) where the parameter should be applied.
 * - Set flags to modify the parameter's behavior (such as making it a secret or authentication-related).
 * - Enable or disable parameters for inclusion in requests.
 * - Merge two parameters with potentially conflicting attributes into a single parameter.
 *
 * \signal void parameterChanged()
 * \brief Emitted when the parameter's value or properties change.
 *
 * \see ParameterScope, ParameterFlag, ParameterFlags
 */

/*!
 * \brief Default constructor for ApiRequestParameter.
 *
 * Initializes an empty API request parameter with default values.
 */
ApiRequestParameter::ApiRequestParameter() :
    d(new ApiRequestParameterPrivate)
{
}

/*!
 * \brief Constructs an ApiRequestParameter with the given name, value, and scope.
 * \param name The name of the parameter.
 * \param value The value of the parameter as a QVariant.
 * \param scope The scope of the parameter (default is UrlQueryScope).
 */
ApiRequestParameter::ApiRequestParameter(const QString &name, const QVariant &value, ParameterScope scope) :
    d(new ApiRequestParameterPrivate)
{
    d->name = name;
    d->value = value;
    d->scope = scope;
}

/*!
 * \brief Copy constructor for ApiRequestParameter.
 * \param other The ApiRequestParameter instance to copy.
 *
 * Duplicates the properties of the given \a other ApiRequestParameter.
 */
ApiRequestParameter::ApiRequestParameter(const ApiRequestParameter &other) :
    d(other.d)
{
}

/*!
 * \brief Move constructor for ApiRequestParameter.
 * \param other The ApiRequestParameter instance to move from.
 *
 * Transfers ownership of the data from \a other to the newly created ApiRequestParameter, leaving \a other in a valid but unspecified state.
 */
ApiRequestParameter::ApiRequestParameter(ApiRequestParameter &&other) :
    d(other.d)
{
}

/*!
 * \brief Destructor for ApiRequestParameter.
 *
 * Cleans up resources held by the ApiRequestParameter object.
 */
ApiRequestParameter::~ApiRequestParameter()
{
}

/*!
 * \brief Assignment operator for ApiRequestParameter.
 * \param other The ApiRequestParameter instance to assign from.
 * \return A reference to this ApiRequestParameter instance.
 *
 * Copies the properties from the given \a other ApiRequestParameter to this object.
 */
ApiRequestParameter &ApiRequestParameter::operator=(const ApiRequestParameter &other)
{
    if (this != &other)
        d = other.d;
    return *this;
}

/*!
 * \brief Move assignment operator for ApiRequestParameter.
 * \param other The ApiRequestParameter instance to move from.
 * \return A reference to this ApiRequestParameter instance.
 *
 * Transfers ownership of the data from \a other to this object, leaving \a other in a valid but unspecified state.
 */
ApiRequestParameter &ApiRequestParameter::operator=(ApiRequestParameter &&other)
{
    if (this != &other)
        d.swap(other.d);
    return *this;
}

/*!
 * \brief Retrieves the name of the parameter.
 * \return The parameter name as a QString.
 */
QString ApiRequestParameter::name() const
{
    return d->name;
}

/*!
 * \brief Sets the name of the parameter.
 * \param name The name to set for the parameter.
 */
void ApiRequestParameter::setName(const QString &name)
{
    d->name = name;
}

/*!
 * \brief Retrieves the value of the parameter.
 * \return The parameter value as a QVariant.
 */
QVariant ApiRequestParameter::value() const
{
    return d->value;
}

/*!
 * \brief Sets the value of the parameter.
 * \param value The value to set for the parameter.
 */
void ApiRequestParameter::setValue(const QVariant &value)
{
    d->value = value;
}

/*!
 * \brief Retrieves the scope of the parameter.
 * \return The scope of the parameter as a ParameterScope.
 */
ApiRequestParameter::ParameterScope ApiRequestParameter::scope() const
{
    return d->scope;
}

/*!
 * \brief Sets the scope of the parameter.
 * \param scope The scope to set (e.g., UrlPathScope, UrlQueryScope, or HeaderScope).
 */
void ApiRequestParameter::setScope(ParameterScope scope)
{
    d->scope = scope;
}

/*!
 * \brief Checks if a specific flag is set for the parameter.
 * \param flag The ParameterFlag to check.
 * \return \c true if the flag is set, otherwise \c false.
 */
bool ApiRequestParameter::hasFlag(ParameterFlag flag) const
{
    return d->flags.testFlag(flag);
}

/*!
 * \brief Retrieves all flags set for the parameter.
 * \return The flags as a ParameterFlags bitmask.
 */
ApiRequestParameter::ParameterFlags ApiRequestParameter::flags() const
{
    return d->flags;
}

/*!
 * \brief Sets or clears a specific flag for the parameter.
 * \param flag The ParameterFlag to modify.
 * \param on \c true to set the flag, \c false to clear it.
 */
void ApiRequestParameter::setFlag(ParameterFlag flag, bool on)
{
    d->flags.setFlag(flag, on);
    d->adaptToFlag(flag);
}

/*!
 * \brief Sets multiple flags for the parameter.
 * \param flags The ParameterFlags bitmask to set.
 */
void ApiRequestParameter::setFlags(const ParameterFlags &flags)
{
    d->flags = flags;
    d->correctFlags();
}

/*!
 * \brief Checks if the parameter is enabled.
 * \return \c true if the parameter is enabled, otherwise \c false.
 */
bool ApiRequestParameter::isEnabled() const
{
    return d->enabled;
}

/*!
 * \brief Enables or disables the parameter.
 * \param enabled \c true to enable the parameter, \c false to disable it.
 * \warning A disabled parameter will not be taken into account during network request generation
 */
void ApiRequestParameter::setEnabled(bool enabled)
{
    d->enabled = enabled;
}

/*!
 * \brief Loads parameter information from a JSON object.
 * \param object The QJsonObject containing the parameter data.
 */
void ApiRequestParameter::loadFromJsonObject(const QJsonObject &object)
{
    d->name = object.value("name").toString();
    d->value = object.value("value").toVariant();

    if (object.contains("scope"))
        d->scope = d->nameScope(object.value("scope").toString());
    else
        d->scope = UrlQueryScope;

    d->flags = NoFlag;
    if (object.contains("authentication"))
        d->flags.setFlag(AuthenticationFlag, object.value("authentication").toBool());
    else if (object.contains("secret"))
        d->flags.setFlag(SecretFlag, object.value("secret").toBool());

    if (object.contains("enabled"))
        d->enabled = object.value("enabled").toBool();
    else
        d->enabled = true;
}

/*!
 * \brief Checks if the parameter is valid.
 * \return \c true if the parameter is valid, otherwise \c false.
 *
 * A parameter is considered valid if it has a non-empty name and a not null value.
 */
bool ApiRequestParameter::isValid() const
{
    return !d->name.isEmpty() && !d->value.isNull();
}

/*!
 * \brief Swaps the contents of this ApiRequestParameter with another.
 * \param other The ApiRequestParameter instance to swap with.
 */
void ApiRequestParameter::swap(ApiRequestParameter &other)
{
    d.swap(other.d);
}

/*!
 * \brief Compares two ApiRequestParameter objects for equality.
 * \param other The ApiRequestParameter instance to compare with.
 * \return \c true if both parameters are equal, otherwise \c false.
 */
bool ApiRequestParameter::operator==(const ApiRequestParameter &other) const
{
    return d->equals(other.d);
}

/*!
 * \brief Merges two ApiRequestParameter objects into a single parameter.
 * \param p1 The first ApiRequestParameter to merge.
 * \param p2 The second ApiRequestParameter to merge.
 * \return A new ApiRequestParameter containing merged properties.
 */
ApiRequestParameter ApiRequestParameter::mergeParameters(const ApiRequestParameter &p1, const ApiRequestParameter &p2)
{
    ApiRequestParameter p = p1;

    // p2 value is prioritary if not null
    if (!p2.value().isNull())
        p.setValue(p2.value());

    // adding flags from p2 while keeping those of p1
    const QList<ParameterFlag> flags = { AuthenticationFlag, SecretFlag };
    for (const ParameterFlag &flag : flags)
    if (p2.hasFlag(flag))
        p.setFlag(flag);

    // enabled state of p1 is overriden by p2 one
    p.setEnabled(p2.isEnabled());

    return p;
}

ApiRequestParameterPrivate::ApiRequestParameterPrivate() :
    scope(ApiRequestParameter::UrlQueryScope),
    enabled(true),
    flags(ApiRequestParameter::NoFlag)
{
}

ApiRequestParameterPrivate::ApiRequestParameterPrivate(const ApiRequestParameterPrivate &other) :
    QSharedData(other),
    name(other.name),
    value(other.value),
    scope(other.scope),
    flags(other.flags),
    enabled(other.enabled)
{
}

ApiRequestParameterPrivate::~ApiRequestParameterPrivate()
{
}

bool ApiRequestParameterPrivate::equals(const ApiRequestParameterPrivate *other) const
{
    return name == other->name
            && value == other->value
            && scope == other->scope
            && flags == other->flags
            && enabled == other->enabled;
}

void ApiRequestParameterPrivate::correctFlags()
{
    if (flags.testFlag(ApiRequestParameter::AuthenticationFlag))
        adaptToFlag(ApiRequestParameter::AuthenticationFlag);
}

void ApiRequestParameterPrivate::adaptToFlag(ApiRequestParameter::ParameterFlag flag)
{
    switch (flag) {
    case ApiRequestParameter::AuthenticationFlag:
        flags |= ApiRequestParameter::SecretFlag;
        break;

    default:
        break;
    }
}

ApiRequestParameter::ParameterScope ApiRequestParameterPrivate::nameScope(const QString &name)
{
    if (name == "URL_PATH")
        return ApiRequestParameter::UrlPathScope;
    else if (name == "URL_QUERY")
        return ApiRequestParameter::UrlQueryScope;
    else if (name == "HEADER")
        return ApiRequestParameter::HeaderScope;
    else
        return ApiRequestParameter::UrlQueryScope;
}

}
