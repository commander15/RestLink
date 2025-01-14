#include "requestinterface.h"

#include <RestLink/request.h>
#include <RestLink/pathparameter.h>
#include <RestLink/queryparameter.h>
#include <RestLink/header.h>

namespace RestLink {

/*!
 * \class RequestInterface
 * \brief A class for storing and managing some HTTP request informations (url path and query parameter along with headers).
 */

/**
 * @brief Checks if a path parameter exists in the request.
 *
 * This function checks whether a path parameter with the given name exists.
 *
 * @param name The name of the path parameter.
 * @return true If the path parameter exists.
 * @return false If the path parameter does not exist.
 */
bool RequestInterface::hasPathParameter(const QString &name) const
{
    return findPathParameter(name) != constPathParameters()->cend();
}

/**
 * @brief Retrieves a specific path parameter.
 *
 * This function retrieves the path parameter for the given name. If the parameter is not found,
 * it returns a default-constructed `PathParameter`.
 *
 * @param name The name of the path parameter.
 * @return The path parameter corresponding to the given name.
 */
PathParameter RequestInterface::pathParameter(const QString &name) const
{
    auto it = findPathParameter(name);
    if (it != constPathParameters()->end())
        return *it;
    return {};
}

/**
 * @brief Retrieves the value of a path parameter.
 *
 * This function returns the value associated with the given path parameter name.
 * If the parameter is not found, an empty QVariant is returned.
 *
 * @param name The name of the path parameter.
 * @return The value of the path parameter, or an empty QVariant if not found.
 */
QVariant RequestInterface::pathParameterValue(const QString &name) const
{
    auto it = findPathParameter(name);
    return it != constPathParameters()->cend() ? it->value() : QVariant{};
}

/**
 * @brief Retrieves the names of all path parameters.
 *
 * This function returns a list of names of all the path parameters in the request.
 *
 * @return A list of names of path parameters.
 */
QStringList RequestInterface::pathParameterNames() const
{
    QStringList names;
    for (const auto &param : *constPathParameters())
        names.append(param.name());
    return names;
}

/**
 * @brief Sets the value of a path parameter.
 *
 * This function sets or updates the value of a path parameter in the request. If the parameter does
 * not exist, it is added.
 *
 * @param name The name of the path parameter.
 * @param value The new value to set for the path parameter.
 */
void RequestInterface::setPathParameter(const QString &name, const QVariant &value)
{
    auto it = findPathParameter(name);
    if (it != mutablePathParameters()->end())
        it->setValue(value);
    else
        mutablePathParameters()->append(PathParameter(name, value));
}

/**
 * @brief Sets a path parameter using a `PathParameter` object.
 *
 * This function sets or updates a path parameter using a `PathParameter` object.
 *
 * @param parameter The `PathParameter` object containing the name and value.
 */
void RequestInterface::setPathParameter(const PathParameter &parameter)
{
    setPathParameter(parameter.name(), parameter.value());
}

/**
 * @brief Unsets (removes) a path parameter from the request.
 *
 * This function removes the path parameter with the given name from the request.
 *
 * @param name The name of the path parameter to remove.
 */
void RequestInterface::unsetPathParameter(const QString &name)
{
    auto it = findPathParameter(name);
    if (it != mutablePathParameters()->end())
        mutablePathParameters()->removeAt(std::distance(mutablePathParameters()->begin(), it));
}

/**
 * @brief Retrieves all path parameters.
 *
 * This function returns a list of all path parameters in the request.
 *
 * @return A list of all path parameters.
 */
QList<PathParameter> RequestInterface::pathParameters() const {
    return *constPathParameters();
}

/**
 * @brief Sets all path parameters.
 *
 * This function sets the list of path parameters to the provided list of parameters.
 *
 * @param parameters A list of path parameters to set.
 */
void RequestInterface::setPathParameters(const QList<PathParameter> &parameters)
{
    *mutablePathParameters() = parameters;
}

/**
 * @brief Checks if a query parameter exists in the request.
 *
 * This function checks whether a query parameter with the given name exists.
 *
 * @param name The name of the query parameter.
 * @return true If the query parameter exists.
 * @return false If the query parameter does not exist.
 */
bool RequestInterface::hasQueryParameter(const QString &name) const
{
    return findQueryParameter(name) != constQueryParameters()->cend();
}

/**
 * @brief Retrieves a specific query parameter.
 *
 * This function retrieves the query parameter for the given name. If the parameter is not found,
 * it returns a default-constructed `QueryParameter`.
 *
 * @param name The name of the query parameter.
 * @return The query parameter corresponding to the given name.
 */
QueryParameter RequestInterface::queryParameter(const QString &name) const
{
    auto it = findQueryParameter(name);
    if (it != constQueryParameters()->cend())
        return *it;
    return {};
}

/**
 * @brief Retrieves the values of a query parameter.
 *
 * This function returns a list of values associated with the given query parameter name.
 * If the parameter is not found, an empty list is returned.
 *
 * @param name The name of the query parameter.
 * @return A list of values of the query parameter, or an empty list if not found.
 */
QList<QVariant> RequestInterface::queryParameterValues(const QString &name) const
{
    auto it = findQueryParameter(name);
    return it != constQueryParameters()->cend() ? it->values() : QList<QVariant>{};
}

/**
 * @brief Retrieves the names of all query parameters.
 *
 * This function returns a list of names of all the query parameters in the request.
 *
 * @return A list of names of query parameters.
 */
QStringList RequestInterface::queryParameterNames() const
{
    QStringList names;
    for (const auto &param : *constQueryParameters())
        names.append(param.name());
    return names;
}

/**
 * @brief Add the value of a query parameter.
 *
 * This function sets or updates the value of a query parameter in the request. If the parameter does
 * not exist, it is added.
 *
 * @param name The name of the query parameter.
 * @param value The new value to set for the query parameter.
 */
void RequestInterface::addQueryParameter(const QString &name, const QVariant &value)
{
    auto it = findQueryParameter(name);
    if (it != mutableQueryParameters()->end())
        it->addValue(value);
    else
        mutableQueryParameters()->append(QueryParameter(name, value));
}

/**
 * @brief Add query parameter using a `QueryParameter` object.
 *
 * This function sets or updates a query parameter using a `QueryParameter` object.
 *
 * @param param The `QueryParameter` object containing the name and value.
 */
void RequestInterface::addQueryParameter(const QueryParameter &param)
{
    auto it = findQueryParameter(param.name());
    if (it != mutableQueryParameters()->end())
        for (const QVariant &value : param.values())
            it->addValue(value);
    else
        mutableQueryParameters()->append(param);
}

/**
 * @brief Removes a query parameter from the request.
 *
 * This function removes the query parameter with the given name from the request.
 *
 * @param name The name of the query parameter to remove.
 */
void RequestInterface::removeQueryParameter(const QString &name)
{
    auto it = findQueryParameter(name);
    if (it != mutableQueryParameters()->end())
        mutableQueryParameters()->removeAt(std::distance(mutableQueryParameters()->begin(), it));
}


/**
 * @brief Removes a query parameter from the request.
 *
 * This function removes the value on the query parameter if it exists.
 *
 * @param name The name of the query parameter to remove.
 */
void RequestInterface::removeQueryParameter(const QString &name, const QVariant &value)
{
    auto it = findQueryParameter(name);
    if (it != mutableQueryParameters()->end())
        it->removeValue(value);
}

/**
 * @brief Retrieves all query parameters.
 *
 * This function returns a list of all query parameters in the request.
 *
 * @return A list of all query parameters.
 */
QList<QueryParameter> RequestInterface::queryParameters() const
{
    return *constQueryParameters();
}

/**
 * @brief Sets all query parameters.
 *
 * This function sets the list of query parameters to the provided list of parameters.
 *
 * @param parameters A list of query parameters to set.
 */
void RequestInterface::setQueryParameters(const QList<QueryParameter> &parameters)
{
    *mutableQueryParameters() = parameters;
}

/**
 * @brief Checks if a header exists in the request.
 *
 * This function checks whether a header with the given name exists.
 *
 * @param name The name of the header.
 * @return true If the header exists.
 * @return false If the header does not exist.
 */
bool RequestInterface::hasHeader(const QString &name) const
{
    return findHeader(name) != constHeaders()->cend();
}

/**
 * @brief Retrieves a specific header.
 *
 * This function retrieves the header for the given name. If the header is not found,
 * it returns a default-constructed `Header`.
 *
 * @param name The name of the header.
 * @return The header corresponding to the given name.
 */
Header RequestInterface::header(const QString &name) const {
    auto it = findHeader(name);
    if (it != constHeaders()->cend())
        return *it;
    return {};
}

/**
 * @brief Retrieves the values of a header.
 *
 * This function returns a list of values associated with the given header name.
 * If the header is not found, an empty list is returned.
 *
 * @param name The name of the header.
 * @return A list of values of the header, or an empty list if not found.
 */
QList<QVariant> RequestInterface::headerValues(const QString &name) const
{
    auto it = findHeader(name);
    return it != constHeaders()->cend() ? it->values() : QList<QVariant>{};
}

/**
 * @brief Retrieves the names of all headers.
 *
 * This function returns a list of names of all the headers in the request.
 *
 * @return A list of names of headers.
 */
QStringList RequestInterface::headerNames() const {
    QStringList names;
    for (const auto &header : *constHeaders())
        names.append(header.name());
    return names;
}

/**
 * @brief Sets the value of a header.
 *
 * This function sets or updates the value of a header in the request. If the header does
 * not exist, it is added.
 *
 * @param name The name of the header.
 * @param value The new value to set for the header.
 */
void RequestInterface::setHeader(const QString &name, const QVariant &value)
{
    auto it = findHeader(name);
    if (it != mutableHeaders()->end())
        it->addValue(value);
    else
        mutableHeaders()->append(Header(name, value));
}

/**
 * @brief Sets a header using a `Header` object.
 *
 * This function sets or updates a header using a `Header` object.
 *
 * @param header The `Header` object containing the name and value.
 */
void RequestInterface::setHeader(const Header &header)
{
    auto it = findHeader(header.name());
    if (it != mutableHeaders()->end())
        *it = header;
    else
        mutableHeaders()->append(header);
}

/**
 * @brief Unsets (removes) a header from the request.
 *
 * This function removes the header with the given name from the request.
 *
 * @param name The name of the header to remove.
 */
void RequestInterface::unsetHeader(const QString &name)
{
    auto it = findHeader(name);
    if (it != mutableHeaders()->end())
        mutableHeaders()->removeAt(std::distance(mutableHeaders()->begin(), it));
}

/**
 * @brief Retrieves all headers.
 *
 * This function returns a list of all headers in the request.
 *
 * @return A list of all headers.
 */
QList<Header> RequestInterface::headers() const
{
    return *constHeaders();
}

/**
 * @brief Sets all headers.
 *
 * This function sets the list of headers to the provided list of headers.
 *
 * @param headers A list of headers to set.
 */
void RequestInterface::setHeaders(const QList<Header> &headers)
{
    *this->mutableHeaders() = headers;
}

/**
 * @brief Finds a path parameter by name.
 *
 * This function searches for a path parameter with the given name in the list of path parameters.
 *
 * @param name The name of the path parameter.
 * @return An iterator pointing to the path parameter if found, otherwise pointing to the end.
 */
QList<PathParameter>::const_iterator RequestInterface::findPathParameter(const QString &name) const
{
    return std::find_if(constPathParameters()->begin(), constPathParameters()->end(), [name](const PathParameter &parameter) {
        return parameter.name() == name;
    });
}

/**
 * @brief Finds a mutable path parameter by name.
 *
 * This function searches for a mutable path parameter with the given name in the list of path parameters.
 *
 * @param name The name of the path parameter.
 * @return An iterator pointing to the path parameter if found, otherwise pointing to the end.
 */
QList<PathParameter>::iterator RequestInterface::findPathParameter(const QString &name)
{
    return std::find_if(mutablePathParameters()->begin(), mutablePathParameters()->end(), [name](const PathParameter &parameter) {
        return parameter.name() == name;
    });
}

/**
 * @brief Finds a query parameter by name.
 *
 * This function searches for a query parameter with the given name in the list of query parameters.
 *
 * @param name The name of the query parameter.
 * @return An iterator pointing to the query parameter if found, otherwise pointing to the end.
 */
QList<QueryParameter>::const_iterator RequestInterface::findQueryParameter(const QString &name) const
{
    return std::find_if(constQueryParameters()->begin(), constQueryParameters()->end(), [name](const QueryParameter &parameter) {
        return parameter.name() == name;
    });
}

/**
 * @brief Finds a mutable query parameter by name.
 *
 * This function searches for a mutable query parameter with the given name in the list of query parameters.
 *
 * @param name The name of the query parameter.
 * @return An iterator pointing to the query parameter if found, otherwise pointing to the end.
 */
QList<QueryParameter>::iterator RequestInterface::findQueryParameter(const QString &name)
{
    return std::find_if(mutableQueryParameters()->begin(), mutableQueryParameters()->end(), [name](const QueryParameter &parameter) {
        return parameter.name() == name;
    });
}

/**
 * @brief Finds a header by name.
 *
 * This function searches for a header with the given name in the list of headers.
 *
 * @param name The name of the header.
 * @return An iterator pointing to the header if found, otherwise pointing to the end.
 */
QList<Header>::const_iterator RequestInterface::findHeader(const QString &name) const
{
    return std::find_if(constHeaders()->begin(), constHeaders()->end(), [name](const Header &header) {
        return header.name() == name;
    });
}

/**
 * @brief Finds a mutable header by name.
 *
 * This function searches for a mutable header with the given name in the list of headers.
 *
 * @param name The name of the header.
 */
QList<Header>::iterator RequestInterface::findHeader(const QString &name)
{
    return std::find_if(mutableHeaders()->begin(), mutableHeaders()->end(), [name](const Header &header) {
        return header.name() == name;
    });
}

} // namespace RestLink
