#include "response.h"
#include "response_p.h"

#include <RestLink/debug.h>
#include <RestLink/httputils.h>
#include <RestLink/private/networkresponse_p.h>

#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>

namespace RestLink {

/*!
 * \class RestLink::Response
 * \brief Represents a response to an API request in the RestLink framework.
 *
 * The Response class encapsulates the response from an API request, providing methods for accessing
 * various aspects of the response such as status, headers, body, and progress. It also provides
 * signals for progress updates, completion, and error handling.
 *
 * The Response object is associated with a specific API request and provides functionality to
 * handle download/upload progress, SSL errors, network errors, and HTTP status codes. It also
 * supports reading the response body as JSON or raw data and provides the option to abort or ignore
 * specific errors during the process.
 *
 * \property QString endpoint
 * \brief The endpoint of the API request.
 *
 * \property RestLink::Api::Method method
 * \brief The HTTP method (e.g., GET, POST) associated with this response.
 *
 * \property QUrl url
 * \brief The URL of the API request.
 *
 * \property bool running
 * \brief Indicates whether the request is currently running.
 *
 * \property bool finished
 * \brief Indicates whether the request has finished.
 *
 * \property bool success
 * \brief Indicates whether the request was successful.
 *
 * \property int httpStatusCode
 * \brief The HTTP status code of the response.
 *
 * \property QString httpReasonPhrase
 * \brief The HTTP reason phrase associated with the status code.
 *
 * \property QByteArrayList headerList
 * \brief The list of headers returned in the response.
 *
 * \property int networkError
 * \brief The network error code, if any.
 *
 * \property QString networkErrorString
 * \brief A string description of the network error, if any.
 *
 * The class also provides methods to:
 * - Read the response body in various formats (JSON, QString, QByteArray).
 * - Monitor and control download and upload progress.
 * - Abort the request or ignore SSL errors.
 * - Access detailed information on any network or HTTP errors.
 *
 * \signal void downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
 * \brief Emitted when download progress is made.
 *
 * \signal void uploadProgress(qint64 bytesSent, qint64 bytesTotal)
 * \brief Emitted when upload progress is made.
 *
 * \signal void finished()
 * \brief Emitted when the request is finished.
 *
 * \signal void readyRead()
 * \brief Emitted when data is ready to be read from the response.
 *
 * \signal void sslErrorsOccured(const QList<QSslError> &errors)
 * \brief Emitted when SSL errors occur during the request.
 *
 * \signal void networkErrorOccured(int error)
 * \brief Emitted when a network error occurs during the request.
 */

Response::Response(ResponsePrivate *d, QObject *parent)
    : ResponseBase(parent)
    , d_ptr(d)
{
}

/*!
 * \brief Destructor for Response.
 *
 * Cleans up resources held by the Response object.
 */
Response::~Response()
{
}

/*!
 * \brief Retrieves the API endpoint associated with this response.
 * \return The endpoint as a QString.
 */
QString Response::endpoint() const
{
    return d_ptr->request.endpoint();
}

/*!
 * \brief Retrieves the API request associated with this response.
 * \return The Request object.
 */
Request Response::request() const
{
    return d_ptr->request;
}

/*!
 * \fn Response::method
 * \brief Retrieves the HTTP method type for this response.
 * \return The HTTP mthod as an Api::Method.
 */

/*!
 * \brief Retrieves the API instance associated with this response.
 * \return A pointer to the Api object.
 */
Api *Response::api() const
{
    return d_ptr->request.api();
}

/*!
 * \brief Retrieves the URL associated with this response.
 * \return The URL as a QUrl object.
 */
QUrl Response::url() const
{
    return d_ptr->request.url();
}

/*!
 * \fn Response::networkRequest
 * \brief Retrieves the network request used in this response.
 * \return The QNetworkRequest object.
 */

/*!
 * \fn Response::isRunning
 * \brief Checks if the request is currently running.
 * \return \c true if the request is running, otherwise \c false.
 */

/*!
 * \fn Response::isFinished
 * \brief Checks if the request has finished processing.
 * \return \c true if the request is finished, otherwise \c false.
 */

/*!
 * \fn Response::isSuccess
 * \brief Checks if the request was successful.
 * \return \c true if the request was successful, otherwise \c false.
 */

/*!
 * \brief Checks if the HTTP status code indicates success.
 * \return \c true if the HTTP status code indicates success, otherwise \c false.
 */
bool Response::isHttpStatusSuccess() const
{
    const int code = httpStatusCode();
    return (code >= 200 && code <= 299);
}

/*!
 * \brief Checks if the HTTP status code is valid.
 * \return \c true if the HTTP status code is valid, otherwise \c false.
 */
bool Response::hasHttpStatusCode() const
{
    const int code = httpStatusCode();
    return code >= 100 && code <= 599;
}

QString Response::httpReasonPhrase() const
{
    return HttpUtils::reasonPhrase(httpStatusCode());
}

/*!
 * \fn Response::httpStatusCode
 * \brief Retrieves the HTTP status code of the response.
 * \return The HTTP status code.
 */

/*!
 * \fn Response::httpReasonPhrase
 * \brief Retrieves the HTTP reason phrase of the response.
 * \return The HTTP reason phrase as a QString.
 */

/*!
 * \brief Checks if the response contains a specific header.
 * \param name The name of the header to check.
 * \return \c true if the header is present, otherwise \c false.
 */
bool Response::hasHeader(const QString &name) const
{
    return headerList().contains(name);
}

/*!
 * \fn Response::header
 * \brief Retrieves the value of a specific header.
 * \param header The name of the header to retrieve.
 * \return The header value as a QString.
 */

/*!
 * \fn Response::headerList
 * \brief Retrieves a list of all headers in the response.
 * \return A QStringList containing the header names.
 */

/*!
 * \fn Response::hasNetworkError
 * \brief Checks if a network error occurred during the request.
 * \return \c true if a network error occurred, otherwise \c false.
 */

/*!
 * \brief Retrieves the network error code.
 * \return The network error code.
 */
int Response::networkError() const
{
    const QNetworkReply *reply = networkReply();
    return (reply ? reply->error() : QNetworkReply::NoError);
}

/*!
 * \brief Retrieves a string description of the network error.
 * \return The network error description as a QString.
 */
QString Response::networkErrorString() const
{
    const QNetworkReply *reply = networkReply();
    return (reply ? reply->errorString() : QString());
}

void Response::setRequest(const Request &request)
{
    d_ptr->request = request;
}

QByteArray Response::body()
{
    d_ptr->body.append(readBody());
    return d_ptr->body;
}

void Response::ignoreSslErrors()
{
    QNetworkReply *reply = networkReply();
    if (reply)
        reply->ignoreSslErrors();
}

ResponsePrivate::ResponsePrivate(Response *q)
    : q_ptr(q)
{
}

}
