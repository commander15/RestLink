#include "request.h"
#include "request_p.h"

#include <RestLink/api.h>

#include <QtNetwork/qhttpheaders.h>

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qurlquery.h>
#include <qnetworkrequest.h>

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
    d_ptr(new RequestPrivate())
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
    d_ptr(new RequestPrivate())
{
    if (endpoint.startsWith('/'))
        d_ptr->endpoint = RequestPrivate::validateEndpoint(endpoint);
    else
        d_ptr->baseUrl = QUrl(endpoint);
}

/*!
 * \brief Construct a Request object with a specified URL.
 *
 * This constructor initialize the Request object with the given URL.
 * The url must be include the scheme i.e starting with http/https.
 *
 * \param url The url as a QUrl.
 *
 * @note The provided url will be used as the base url, you can then launch the request directly on a RequestHandler
 * without an Api object.
 */
Request::Request(const QUrl &url)
    : d_ptr(new RequestPrivate())
{
    d_ptr->baseUrl = url;
}

Request::Request(const RequestProcessing &processing) :
    d_ptr(new RequestPrivate())
{
    d_ptr->processing = processing;
}

Request::Request(RequestPrivate *d)
    : d_ptr(d)
{
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
    d_ptr->endpoint = RequestPrivate::validateEndpoint(endpoint);
}

/*!
 * \brief Retrieves the API base url associated with this request.
 * \return  The base url as a QUrl.
 * \note If not base url has been explicitly set or is invalid,
 * this method return the base url of the associated Api if it has been set.
 */
QUrl Request::baseUrl() const
{
    if (!d_ptr->baseUrl.isValid() && d_ptr->api)
        return d_ptr->api->url();

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
    QUrl url = baseUrl();

    Api *api = d_ptr->api;

    auto canUseParameter = [&type](const Parameter &param) {
        return RequestPrivate::canUseUrlParameter(param, type);
    };

    auto parameterValue = [&api](const Parameter &param) {
        return (api ? param.specialValue(api) : param.value());
    };

    auto parameterValues = [&api](const Parameter &param) {
        return (api ? param.specialValues(api) : param.values());
    };

    QString path = d_ptr->endpoint;
    const PathParameterList apiPathParameters = (api ? api->pathParameters() : PathParameterList());
    const PathParameterList pathParameters = d_ptr->pathParameters + apiPathParameters;
    for (const PathParameter &parameter : pathParameters)
        if (canUseParameter(parameter))
            path.replace('{' + parameter.name() + '}', parameterValue(parameter).toString());
    url.setPath(url.path() + path);

    QUrlQuery query(url.query());
    const QueryParameterList apiQueryParameters = (api ? api->queryParameters() : QueryParameterList());
    const QueryParameterList queryParameters = d_ptr->queryParameters + apiQueryParameters;
    for (const QueryParameter &parameter : queryParameters) {
        if (!canUseParameter(parameter))
            continue;

        const QVariantList values = parameterValues(parameter);
        for (const QVariant &value : values)
            query.addQueryItem(parameter.name(), value.toString());
    }
    url.setQuery(query);

    return url;
}

/*!
 * \brief Constructs the URL path for the request based on its parameters and endpoint.
 * \return The constructed URL path as a QString.
 * \deprecated
 */
QString Request::urlPath() const
{
    return url().path();
}

/*!
 * \brief Return the corresponding http headers for the request.
 * \return The corresponding http headers for the request.
 */
QHttpHeaders Request::httpHeaders() const
{
    QHttpHeaders httpHeaders;

    Api *api = d_ptr->api;

    const HeaderList apiHeaders = (api ? api->headers() : HeaderList());
    const HeaderList headers = d_ptr->headers + apiHeaders;
    for (const Header &header : headers) {
        if (httpHeaders.contains(header.name()))
            httpHeaders.removeAll(header.name());

        const QVariantList values = header.values();
        for (const QVariant &value : values)
            httpHeaders.append(header.name(), value.toByteArray());
    }

    if (!api)
        return httpHeaders;

    // Bearer token
    const QString bearerToken = api->bearerToken();
    if (!bearerToken.isEmpty())
        httpHeaders.append(QHttpHeaders::WellKnownHeader::Authorization, "Bearer " + bearerToken);

    // User Agent override
    httpHeaders.append(QHttpHeaders::WellKnownHeader::UserAgent, api->userAgent());

    // Accept language
    if (api) {
        const QString full = api->locale().name();
        const QString slim = full.section('_', 0, 0);
        httpHeaders.append(QHttpHeaders::WellKnownHeader::AcceptLanguage, QStringLiteral("%1,%2;q=0.5").arg(full, slim).toLatin1());
    }

    return httpHeaders;
}

/*!
 * \brief Returns the attribute associated with the code code.
 * \param attribute The attribute for which we want value.
 * \return The attribute value as a QVariant
 */
QVariant Request::attribute(Attribute attribute) const
{
    return d_ptr->attributes.value(attribute);
}

/*!
 * \brief Returns the attribute associated with the code code.
 * Return defaultValue if the attribute has not been set.
 *
 * \param attribute The attribute for which we want value.
 * \return The attribute value as a QVariant
 */
QVariant Request::attribute(Attribute attribute, const QVariant &defaultValue) const
{
    return (d_ptr->attributes.contains(attribute) ? d_ptr->attributes.value(attribute) : defaultValue);
}

/*!
 * \brief Set attribute value to the provided one.
 * \param attribute The attribute for which value must ve set.
 * \param value The value of the attribute to set.
 */
void Request::setAttribute(Attribute attribute, const QVariant &value)
{
    if (value.isValid())
        d_ptr->attributes.insert(attribute, value);
    else if (d_ptr->attributes.contains(attribute))
        d_ptr->attributes.remove(attribute);
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
 * \brief Get the controller associated with the request.
 */
AbstractController *Request::controller() const
{
    return d_ptr->controller;
}

/*!
 * \brief Associate a controller to the request.
 * \param controller The controller to set.
 * \note The controller will be used to process the request in meantime except if it's used with
 * a special RestLink endpoint such as /restlink/register-controller.
 */
void Request::setController(AbstractController *controller)
{
    d_ptr->controller = controller;
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

    RequestPrivate *data = request.d_ptr;
    data->endpoint = RequestPrivate::validateEndpoint(object.value("endpoint").toString());
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

QString RequestPrivate::validateEndpoint(const QString &input)
{
    return input;
}

bool RequestPrivate::canUseUrlParameter(const Parameter &parameter, Request::UrlType type)
{
    if (parameter.hasFlag(Parameter::Secret) && type == Request::PublicUrl)
        return false;

    return true;
}

}

template<>
RestLink::RequestPrivate *QSharedDataPointer<RestLink::RequestPrivate>::clone()
{
    return d->clone();
}
