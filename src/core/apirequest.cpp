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

QString ApiRequest::endpoint(bool resolveParameters) const
{
    if (resolveParameters) {
        QStringList paths = d->endpoint.split('/');
        for (QString &path : paths)
            for (const ApiRequestParameter &param : d->parameters)
                if (param.scope() == ApiRequestParameter::UrlPathScope && path == '{' + param.name() + '}')
                    path = param.value().toString();
        return paths.join('/');
    } else
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
    if (!parameter.name().isEmpty())
        d->parameters.append(parameter);
}

void ApiRequest::removeParameter(const QString &name)
{
    for (const ApiRequestParameter &parameter : d->parameters) {
        if (parameter.name() == name) {
            d->parameters.removeOne(parameter);
            break;
        }
    }
}

void ApiRequest::setParameters(const QList<ApiRequestParameter> &parameters)
{
    d->parameters = parameters.toVector();
}

QByteArray ApiRequest::data() const
{
    return d->data;
}

ApiRequest::DataType ApiRequest::dataType() const
{
    return d->dataType;
}

void ApiRequest::setRawData(const QByteArray &data)
{
    d->data = data;
    d->dataType = RawData;
}

void ApiRequest::setFileName(const QString &fileName)
{
    d->data = fileName.toUtf8();
    d->dataType = FileData;
}

void ApiRequest::setData(const QByteArray &data, DataType type)
{
    d->data = data;
    d->dataType = type;
}

ApiRequest::RequestVerb ApiRequest::verb() const
{
    return d->verb;
}

void ApiRequest::setVerb(RequestVerb verb)
{
    d->verb = verb;
}

void ApiRequest::loadFromJsonObject(const QJsonObject &object)
{
    ApiRequestPrivate *def = new ApiRequestPrivate;

    d->endpoint = object.value("endpoint").toString();

    QJsonArray parameters = object.value("parameters").toArray();
    d->parameters.clear();
    for (const QJsonValue &parameterValue : parameters) {
        ApiRequestParameter requestParameter;
        requestParameter.loadFromJsonObject(parameterValue.toObject());
        d->parameters.append(requestParameter);
    }

    d->data = object.value("data").toString().toUtf8();

    if (object.contains("verb"))
        d->verb = d->nameVerb(object.value("verb").toString());
    else
        d->verb = def->verb;

    // ToDo: verb loading

    delete def;
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

ApiRequestPrivate::ApiRequestPrivate() :
    verb(ApiRequest::GetRequest)
{
}

ApiRequestPrivate::ApiRequestPrivate(const ApiRequestPrivate &other) :
    QSharedData(other),
    endpoint(other.endpoint),
    parameters(other.parameters),
    data(other.data),
    verb(other.verb)
{
}

ApiRequestPrivate::~ApiRequestPrivate()
{
}

bool ApiRequestPrivate::equals(const ApiRequestPrivate *other) const
{
    return endpoint == other->endpoint
            && parameters == other->parameters
            && data == other->data
            && verb == other->verb;
}

ApiRequest::RequestVerb ApiRequestPrivate::nameVerb(const QString &name)
{
    if (name == "GET")
        return ApiRequest::GetRequest;
    else if (name == "POST")
        return ApiRequest::PostRequest;
    else if (name == "PUT")
        return ApiRequest::PutRequest;
    else if (name == "PATCH")
        return ApiRequest::PatchRequest;
    else if (name == "DELETE")
        return ApiRequest::DeleteRequest;
    else
        return ApiRequest::UnknownRequest;
}

ApiRequestParameter::ApiRequestParameter() :
    d(new ApiRequestParameterPrivate)
{
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
}

void ApiRequestParameter::setFlags(const ParameterFlags &flags)
{
    d->flags = flags;
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
    ApiRequestParameterPrivate *def = new ApiRequestParameterPrivate;

    d->name = object.value("name").toString();
    d->value = object.value("value").toVariant();

    if (object.contains("scope"))
        d->scope = d->nameScope(object.value("scope").toString());
    else
        d->scope = def->scope;

    d->flags = def->flags;
    if (object.contains("authentication"))
        d->flags.setFlag(AuthenticationFlag, object.value("authentication").toBool());
    else if (object.contains("secret"))
        d->flags.setFlag(SecretFlag, object.value("secret").toBool());
    d->correctFlags();

    if (object.contains("enabled"))
        d->enabled = object.value("enabled").toBool();
    else
        d->enabled = def->enabled;

    delete def;
}

bool ApiRequestParameter::isValid() const
{
    return !d->name.isEmpty()
            && !d->value.isNull();
}

void ApiRequestParameter::swap(ApiRequestParameter &other)
{
    d.swap(other.d);
}

bool ApiRequestParameter::operator==(const ApiRequestParameter &other) const
{
    return d->equals(other.d);
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
        return ApiRequestParameter::UnknownScope;
}

}
