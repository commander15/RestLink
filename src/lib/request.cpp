#include "request.h"
#include "request_p.h"

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qurlquery.h>

namespace RestLink {

/*!
 * \class Request
 * \brief A class for storing and managing HTTP request information.
 *
 * The Request class encapsulates the essential details of an HTTP request,
 * such as the endpoint, HTTP method, headers, query parameters, and more.
 * It provides a convenient way to define and organize API requests before
 * execution using RestLink.
 */

/*!
 * \brief Default constructor for Request.
 *
 * Initializes an empty Request object. This constructor sets up the
 * object with default values for its members, ensuring a consistent state.
 */
Request::Request() :
    d_ptr(new RequestData())
{
}

/**
 * @brief Constructs an Request object with a specified endpoint.
 *
 * This constructor initializes the Request object with the given API endpoint.
 * The endpoint will be used for making requests. Other parameters (such as headers
 * or query parameters) will be empty by default.
 *
 * @param endpoint The API endpoint as a C-style string (const char*).
 *
 * \note endpoint must not include full path, only the url path relative to the base url provided on Api object.
 */
Request::Request(const char *endpoint) :
    Request(QString::fromUtf8(endpoint, -1))
{
}

/**
 * @brief Constructs an Request object with a specified endpoint.
 *
 * This constructor initializes the Request object with the given API endpoint.
 * The endpoint will be used for making requests. Other parameters (such as headers
 * or query parameters) will be empty by default.
 *
 * @param endpoint The API endpoint as a QString.
 *
 * \note endpoint must not include full path, only the url path relative to the base url provided on Api object.
 */
Request::Request(const QString &endpoint) :
    d_ptr(new RequestData())
{
    d_ptr->endpoint = RequestData::validateEndpoint(endpoint);
}

Request::Request(const RequestProcessing &processing) :
    d_ptr(new RequestData())
{
    d_ptr->processing = processing;
}

/*!
 * \brief Copy constructor for Request.
 *
 * Creates a new Request object as a copy of another. All data, including
 * headers, URL, and parameters, are duplicated into the new instance.
 *
 * \param other The Request object to copy.
 */
Request::Request(const Request &other) :
    d_ptr(other.d_ptr)
{
}

/*!
 * \brief Move constructor for Request.
 *
 * Transfers ownership of the data from another Request object to this one.
 * The source object is left in a valid but unspecified state.
 *
 * \param other The Request object to move from.
 */
Request::Request(Request &&other) :
    d_ptr(other.d_ptr)
{
}

/*!
 * \brief Destructor for Request.
 *
 * Cleans up any resources used by the Request object. Ensures all dynamically
 * allocated memory is released.
 */
Request::~Request()
{
}

/*!
 * \brief Copy assignment operator for Request.
 *
 * Copies the content of another Request object to this one, replacing the
 * existing data. Properly handles self-assignment.
 *
 * \param other The Request object to copy.
 * \return A reference to the updated Request object.
 */
Request &Request::operator=(const Request &other)
{
    if (this != &other)
        d_ptr = other.d_ptr;
    return *this;
}

/*!
 * \brief Move assignment operator for Request.
 *
 * Transfers ownership of the data from another Request object to this one.
 * The existing data in this object is replaced, and the source object is left
 * in a valid but unspecified state.
 *
 * \param other The Request object to move from.
 * \return A reference to the updated Request object.
 */
Request &Request::operator=(const Request &&other)
{
    if (this != &other)
        d_ptr = std::move(other.d_ptr);
    return *this;
}

/*!
 * \brief Retrieves the API endpoint associated with this request.
 * \return The endpoint as a QString.
 */
QString Request::endpoint() const
{
    return d_ptr->endpoint;
}

/*!
 * \brief Set the API endpoint associated with this request.
 * \param endpoint The endpoint as a QString.
 */
void Request::setEndpoint(const QString &endpoint)
{
    d_ptr->endpoint = RequestData::validateEndpoint(endpoint);
}

/*!
 * \brief Retrieves the API base url associated with this request.
 * \return  The base url as a QUrl.
 */
QUrl Request::baseUrl() const
{
    return d_ptr->baseUrl;
}

/*!
 * \brief Set the API base url associated with this request.
 * \param url The new base url
 */
void Request::setBaseUrl(const QUrl &url)
{
    d_ptr->baseUrl = url;
}

/*!
 * \brief Construct the full URL for the request.
 * \param type The url type to return.
 * \return The URL as a QUrl.
 */
QUrl Request::url(UrlType type) const
{
    QUrl url = d_ptr->baseUrl;

    QString path = d_ptr->endpoint;
    for (const PathParameter &parameter : d_ptr->pathParameters)
        if (!parameter.flags().testFlags(Parameter::Secret))
            path.replace('{' + parameter.name() + '}', parameter.value().toString());
    url.setPath(url.path() + path);

    QUrlQuery query(url.query());
    for (const QueryParameter &parameter : d_ptr->queryParameters)
        if (!parameter.flags().testFlags(Parameter::Secret))
            query.addQueryItem(parameter.name(), parameter.value().toString());
    url.setQuery(query);

    return url;
}

/*!
 * \brief Constructs the URL path for the request based on its parameters and endpoint.
 * \return The constructed URL path as a QString.
 */
QString Request::urlPath() const
{
    QString path = d_ptr->endpoint;
    for (const PathParameter &param : d_ptr->pathParameters)
        path.replace('{' + param.name() + '}', param.value().toString());
    return path;
}

RequestProcessing Request::processing() const
{
    return d_ptr->processing;
}

void Request::setProcessing(RequestProcessing processing)
{
    d_ptr->processing = processing;
}

/*!
 * \brief return the Api associated to this request
 * \return The Api.
 */
Api *Request::api() const
{
    return d_ptr->api;
}

/*!
 * \brief Associate an Api to this request
 * \param api The Api to associate
 */
void Request::setApi(Api *api)
{
    d_ptr->api = api;
}

QJsonObject Request::toJsonObject() const
{
    QJsonObject parameters;
    {
        QJsonArray pathParameterArray;
        for (const PathParameter &param : d_ptr->pathParameters) {
            QJsonObject paramObject;
            paramObject.insert("name", param.name());
            paramObject.insert("value", QJsonValue::fromVariant(param.value()));
            pathParameterArray.append(paramObject);
        }

        QJsonArray queryParameterArray;
        for (const QueryParameter &param : d_ptr->queryParameters)
            pathParameterArray.append(param.toJsonObject());

        if (!pathParameterArray.isEmpty())
            parameters.insert("path", pathParameterArray);

        if (!queryParameterArray.isEmpty())
            parameters.insert("query", queryParameterArray);
    }

    QJsonArray headerArrays;
    for (const Header &header : d_ptr->headers)
        headerArrays.append(header.toJsonObject());

    QJsonObject object;
    object.insert("endpoint", d_ptr->endpoint);
    object.insert("base_url", d_ptr->baseUrl.toString());
    if (!parameters.isEmpty())
        object.insert("parameters", parameters);
    if (!headerArrays.isEmpty())
        object.insert("headers", headerArrays);
    return object;
}

Request Request::fromJsonbject(const QJsonObject &object)
{
    Request request;

    RequestData *data = request.d_ptr;
    data->endpoint = RequestData::validateEndpoint(object.value("endpoint").toString());
    data->baseUrl = QUrl(object.value("base_url").toString());

    if (object.contains("parameters")) {
        const QJsonObject parameters = object.value("parameters").toObject();

        const QJsonArray pathParameterArray = parameters.value("path").toArray();
        for (const QJsonValue &value : pathParameterArray)
            data->pathParameters.append(PathParameter::fromJsonObject(value.toObject()));

        const QJsonArray queryParameterArray = parameters.value("query").toArray();
        for (const QJsonValue &value : queryParameterArray)
            data->queryParameters.append(QueryParameter::fromJsonObject(value.toObject()));
    }

    const QJsonArray headerArray = object.value("headers").toArray();
    for (const QJsonValue &value : headerArray)
        data->headers.append(Header::fromJsonObject(value.toObject()));

    return request;
}

/*!
 * \brief Swaps the contents of this Request with another.
 * \param other The Request instance to swap with.
 */
void Request::swap(Request &other)
{
    d_ptr.swap(other.d_ptr);
}

/*!
 * \brief Merges two Request objects into a single request.
 * \param r1 The first Request to merge.
 * \param r2 The second Request to merge.
 * \return A new Request containing merged properties and parameters.
 */
Request Request::merge(const Request &r1, const Request &r2)
{
    Request request = r1;

    // Adding base url from r2 if not present on r1
    if (!request.baseUrl().isEmpty())
        request.setBaseUrl(r2.baseUrl());

    // Adding path parameters that doesn't exists on r1 from r2 to r1
    const QList<PathParameter> parameters = r2.pathParameters();
    for (const PathParameter &param : parameters) {
        if (!request.hasPathParameter(param.name()))
            request.setPathParameter(param.name(), param.value());
    }

    // Adding query parameters from r2 to r1
    const QList<QueryParameter> parameters2 = r2.queryParameters();
    for (const QueryParameter &param : parameters2) {
        request.addQueryParameter(param);
    }

    // Adding mutableHeaders from r2 to r1
    const QList<Header> headers = r2.headers();
    for (const Header &header : headers) {
        request.setHeader(header);
    }

    if (!request.api())
        request.setApi(r2.api());

    return request;
}

const QList<PathParameter> *Request::constPathParameters() const
{
    return &d_ptr->pathParameters;
}

QList<PathParameter> *Request::mutablePathParameters()
{
    return &d_ptr->pathParameters;
}

const QList<QueryParameter> *Request::constQueryParameters() const
{
    return &d_ptr->queryParameters;
}

QList<QueryParameter> *Request::mutableQueryParameters()
{
    return &d_ptr->queryParameters;
}

const QList<Header> *Request::constHeaders() const
{
    return &d_ptr->headers;
}

QList<Header> *Request::mutableHeaders()
{
    return &d_ptr->headers;
}

QString RequestData::validateEndpoint(const QString &input)
{
    return input;
}

}
