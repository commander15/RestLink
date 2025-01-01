#include "requestinterface.h"

#include <RestLink/request.h>
#include <RestLink/pathparameter.h>
#include <RestLink/queryparameter.h>
#include <RestLink/header.h>

namespace RestLink {

bool RequestInterface::hasPathParameter(const QString &name) const
{
    return findPathParameter(name) != constPathParameters()->cend();
}

PathParameter RequestInterface::pathParameter(const QString &name) const
{
    auto it = findPathParameter(name);
    if (it != constPathParameters()->end())
        return *it;
    return {};
}

QVariant RequestInterface::pathParameterValue(const QString &name) const
{
    auto it = findPathParameter(name);
    return it != constPathParameters()->cend() ? it->value() : QVariant{};
}

QStringList RequestInterface::pathParameterNames() const
{
    QStringList names;
    for (const auto &param : *constPathParameters())
        names.append(param.name());
    return names;
}

void RequestInterface::setPathParameter(const QString &name, const QVariant &value)
{
    auto it = findPathParameter(name);
    if (it != mutablePathParameters()->end())
        it->setValue(value);
    else
        mutablePathParameters()->append(PathParameter(name, value));
}

void RequestInterface::setPathParameter(const PathParameter &parameter)
{
    setPathParameter(parameter.name(), parameter.value());
}

QList<PathParameter> RequestInterface::pathParameters() const {
    return *constPathParameters();
}

void RequestInterface::setPathParameters(const QList<PathParameter> &parameters)
{
    *mutablePathParameters() = parameters;
}

bool RequestInterface::hasQueryParameter(const QString &name) const
{
    return findQueryParameter(name) != constQueryParameters()->cend();
}

QueryParameter RequestInterface::queryParameter(const QString &name) const
{
    auto it = findQueryParameter(name);
    if (it != constQueryParameters()->cend())
        return *it;
    return {};
}

QVariantList RequestInterface::queryParameterValues(const QString &name) const
{
    auto it = findQueryParameter(name);
    return it != constQueryParameters()->cend() ? it->values() : QVariantList{};
}

QStringList RequestInterface::queryParameterNames() const
{
    QStringList names;
    for (const auto &param : *constQueryParameters())
        names.append(param.name());
    return names;
}

void RequestInterface::setQueryParameter(const QString &name, const QVariant &value)
{
    auto it = findQueryParameter(name);
    if (it != mutableQueryParameters()->end())
        it->addValue(value);
    else
        mutableQueryParameters()->append(QueryParameter(name, value));
}

void RequestInterface::setQueryParameter(const QueryParameter &param)
{
    auto it = findQueryParameter(param.name());
    if (it != mutableQueryParameters()->end())
        *it = param;
    else
        mutableQueryParameters()->append(param);
}

QList<QueryParameter> RequestInterface::queryParameters() const
{
    return *constQueryParameters();
}

void RequestInterface::setQueryParameter(const QList<QueryParameter> &parameters)
{
    *mutableQueryParameters() = parameters;
}

bool RequestInterface::hasHeader(const QString &name) const
{
    return findHeader(name) != constHeaders()->cend();
}

Header RequestInterface::header(const QString &name) const {
    auto it = findHeader(name);
    if (it != constHeaders()->cend())
        return *it;
    return {};
}

QVariantList RequestInterface::headerValues(const QString &name) const
{
    auto it = findHeader(name);
    return it != constHeaders()->cend() ? it->values() : QVariantList{};
}

QStringList RequestInterface::headerNames() const {
    QStringList names;
    for (const auto &header : *constHeaders())
        names.append(header.name());
    return names;
}

void RequestInterface::setHeader(const QString &name, const QVariant &value)
{
    auto it = findHeader(name);
    if (it != mutableHeaders()->end())
        it->addValue(value);
    else
        mutableHeaders()->append(Header(name, value));
}

void RequestInterface::setHeader(const Header &header)
{
    auto it = findHeader(header.name());
    if (it != mutableHeaders()->end())
        *it = header;
    else
        mutableHeaders()->append(header);
}

QList<Header> RequestInterface::headers() const
{
    return *constHeaders();
}

void RequestInterface::setHeaders(const QList<Header> &headers)
{
    *this->mutableHeaders() = headers;
}

QList<PathParameter>::const_iterator RequestInterface::findPathParameter(const QString &name) const
{
    return std::find_if(constPathParameters()->begin(), constPathParameters()->end(), [name](const PathParameter &parameter) {
        return parameter.name() == name;
    });
}

QList<PathParameter>::iterator RequestInterface::findPathParameter(const QString &name)
{
    return std::find_if(mutablePathParameters()->begin(), mutablePathParameters()->end(), [name](const PathParameter &parameter) {
        return parameter.name() == name;
    });
}

QList<QueryParameter>::const_iterator RequestInterface::findQueryParameter(const QString &name) const
{
    return std::find_if(constQueryParameters()->begin(), constQueryParameters()->end(), [name](const QueryParameter &parameter) {
        return parameter.name() == name;
    });
}

QList<QueryParameter>::iterator RequestInterface::findQueryParameter(const QString &name)
{
    return std::find_if(mutableQueryParameters()->begin(), mutableQueryParameters()->end(), [name](const QueryParameter &parameter) {
        return parameter.name() == name;
    });
}

QList<Header>::const_iterator RequestInterface::findHeader(const QString &name) const
{
    return std::find_if(constHeaders()->begin(), constHeaders()->end(), [name](const Header &header) {
        return header.name() == name;
    });
}

QList<Header>::iterator RequestInterface::findHeader(const QString &name)
{
    return std::find_if(mutableHeaders()->begin(), mutableHeaders()->end(), [name](const Header &header) {
        return header.name() == name;
    });
}

} // namespace RestLink
