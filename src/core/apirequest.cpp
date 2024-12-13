#include "apirequest.h"
#include "apirequest_p.h"

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace RestLink {

ApiRequest::ApiRequest() :
    d(new ApiRequestPrivate)
{
}

ApiRequest::ApiRequest(const ApiRequest &other) :
    d(other.d)
{
}

ApiRequest::ApiRequest(ApiRequest &&other) :
    d(other.d)
{
}

ApiRequest::~ApiRequest()
{
}

ApiRequest &ApiRequest::operator=(const ApiRequest &other)
{
    if (this != &other)
        d = other.d;
    return *this;
}

ApiRequest &ApiRequest::operator=(ApiRequest &&other)
{
    if (this != &other)
        d.swap(other.d);
    return *this;
}

QString ApiRequest::endpoint() const
{
    return d->endpoint;
}

void ApiRequest::setEndpoint(const QString &endpoint)
{
    if (endpoint.isEmpty())
        d->endpoint.clear();
    else if (!endpoint.startsWith('/'))
        d->endpoint = '/' + endpoint;
    else
        d->endpoint = endpoint;
}

bool ApiRequest::hasParameter(const QString &name) const
{
    for (const ApiRequestParameter &parameter : d->parameters)
        if (parameter.name() == name)
            return true;
    return false;
}

ApiRequestParameter ApiRequest::parameter(const QString &name) const
{
    for (const ApiRequestParameter &parameter : d->parameters)
        if (parameter.name() == name)
            return parameter;
    return ApiRequestParameter();
}

QStringList ApiRequest::parameterList() const
{
    QStringList parameters;
    for (const ApiRequestParameter &parameter : d->parameters)
        parameters.append(parameter.name());
    return parameters;
}

QList<ApiRequestParameter> ApiRequest::parameters() const
{
    return d->parameters.toList();
}

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

void ApiRequest::removeParameter(const QString &name)
{
    d->parameters.removeIf([&name](const ApiRequestParameter &p) {
        return p.name() == name;
    });
}

void ApiRequest::setParameters(const QList<ApiRequestParameter> &parameters)
{
    d->parameters = parameters.toVector();
}

QString ApiRequest::contentType() const
{
    return d->contentType;
}

void ApiRequest::setContentType(const QString &type)
{
    d->contentType = type;
}

bool ApiRequest::isCacheable() const
{
    return d->cache;
}

void ApiRequest::setCacheable(bool cache)
{
    d->cache = cache;
}

QString ApiRequest::urlPath() const
{
    QStringList paths = d->endpoint.split('/');
    for (QString &path : paths)
        for (const ApiRequestParameter &param : d->parameters)
            if (param.scope() == ApiRequestParameter::UrlPathScope && path == '{' + param.name() + '}')
                path = param.value().toString();
    return paths.join('/');
}

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

void ApiRequest::swap(ApiRequest &other)
{
    d.swap(other.d);
}

bool ApiRequest::operator==(const ApiRequest &other) const
{
    if (this == &other)
        return true;
    else
        return d->equals(other.d);
}

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

ApiRequestParameter::ApiRequestParameter() :
    d(new ApiRequestParameterPrivate)
{
}

ApiRequestParameter::ApiRequestParameter(const QString &name, const QVariant &value, ParameterScope scope) :
    d(new ApiRequestParameterPrivate)
{
    d->name = name;
    d->value = value;
    d->scope = scope;
}

ApiRequestParameter::ApiRequestParameter(const ApiRequestParameter &other) :
    d(other.d)
{
}

ApiRequestParameter::ApiRequestParameter(ApiRequestParameter &&other) :
    d(other.d)
{
}

ApiRequestParameter::~ApiRequestParameter()
{
}

ApiRequestParameter &ApiRequestParameter::operator=(const ApiRequestParameter &other)
{
    if (this != &other)
        d = other.d;
    return *this;
}

ApiRequestParameter &ApiRequestParameter::operator=(ApiRequestParameter &&other)
{
    if (this != &other)
        d.swap(other.d);
    return *this;
}

QString ApiRequestParameter::name() const
{
    return d->name;
}

void ApiRequestParameter::setName(const QString &name)
{
    d->name = name;
}

QVariant ApiRequestParameter::value() const
{
    return d->value;
}

void ApiRequestParameter::setValue(const QVariant &value)
{
    d->value = value;
}

ApiRequestParameter::ParameterScope ApiRequestParameter::scope() const
{
    return d->scope;
}

void ApiRequestParameter::setScope(ParameterScope scope)
{
    d->scope = scope;
}

bool ApiRequestParameter::hasFlag(ParameterFlag flag) const
{
    return d->flags.testFlag(flag);
}

ApiRequestParameter::ParameterFlags ApiRequestParameter::flags() const
{
    return d->flags;
}

void ApiRequestParameter::setFlag(ParameterFlag flag, bool on)
{
    d->flags.setFlag(flag, on);
    d->adaptToFlag(flag);
}

void ApiRequestParameter::setFlags(const ParameterFlags &flags)
{
    d->flags = flags;
    d->correctFlags();
}

bool ApiRequestParameter::isEnabled() const
{
    return d->enabled;
}

void ApiRequestParameter::setEnabled(bool enabled)
{
    d->enabled = enabled;
}

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

bool ApiRequestParameter::isValid() const
{
    return !d->name.isEmpty() && !d->value.isNull();
}

void ApiRequestParameter::swap(ApiRequestParameter &other)
{
    d.swap(other.d);
}

bool ApiRequestParameter::operator==(const ApiRequestParameter &other) const
{
    return d->equals(other.d);
}

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
