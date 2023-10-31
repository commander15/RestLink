#include "apirequest.h"
#include "apirequest_p.h"

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

ApiRequestParameter::ApiRequestParameterScope ApiRequestParameter::scope() const
{
    return d->scope;
}

void ApiRequestParameter::setScope(ApiRequestParameterScope scope)
{
    d->scope = scope;
}

bool ApiRequestParameter::isEnabled() const
{
    return d->enabled;
}

void ApiRequestParameter::setEnabled(bool enabled)
{
    d->enabled = enabled;
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
    enabled(true)
{
}

ApiRequestParameterPrivate::ApiRequestParameterPrivate(const ApiRequestParameterPrivate &other) :
    QSharedData(other),
    name(other.name),
    value(other.value),
    scope(other.scope),
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
            && enabled == other->enabled;
}

}
