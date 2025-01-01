#include "response.h"
#include "response_p.h"

#include <RestLink/debug.h>
#include <RestLink/compressionutils.h>

#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

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
 * \property RestLink::Api::Operation operation
 * \brief The HTTP operation (e.g., GET, POST) associated with this response.
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

/*!
 * \brief Protected constructor for Response.
 * \param api A pointer to the Api instance that initiated the request.
 */
Response::Response(const Request &request, QNetworkReply *reply, Api *api)
    : QObject{api}
    , d_ptr(new ResponsePrivate{this})
{
    d_ptr->request = request;

    d_ptr->netReply = reply;
    d_ptr->netReply->setParent(this);

    connect(reply, &QNetworkReply::readyRead, this, &Response::readyRead);
    connect(reply, &QNetworkReply::downloadProgress, this, &Response::downloadProgress);
    connect(reply, &QNetworkReply::uploadProgress, this, &Response::uploadProgress);
    connect(reply, &QNetworkReply::sslErrors, this, &Response::sslErrorsOccured);
    connect(reply, &QNetworkReply::errorOccurred, this, &Response::networkErrorOccured);
    connect(reply, &QNetworkReply::finished, this, &Response::finished);

    d_ptr->api = api;
}

void Response::ignoreSslErrors()
{
    d_ptr->netReply->ignoreSslErrors();
}

void Response::abort()
{
    d_ptr->netReply->abort();
}

QByteArray Response::body()
{
    if (d_ptr->netReply->bytesAvailable() > 0)
        d_ptr->body.append(readBody());
    return d_ptr->body;
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
 * \brief Retrieves the HTTP operation type for this response.
 * \return The HTTP operation as an Api::Operation.
 */
ApiBase::Operation Response::operation() const
{
    switch (d_ptr->netReply->operation()) {
    case QNetworkAccessManager::GetOperation:
        return Api::GetOperation;

    case QNetworkAccessManager::PostOperation:
        return Api::PostOperation;

    case QNetworkAccessManager::PutOperation:
        return Api::PutOperation;

    case QNetworkAccessManager::DeleteOperation:
        return Api::DeleteOperation;

    case QNetworkAccessManager::CustomOperation:
        return Api::PatchOperation;

    default:
        return Api::UnknownOperation;
    }
}

/*!
 * \brief Retrieves the API instance associated with this response.
 * \return A pointer to the Api object.
 */
Api *Response::api() const
{
    return d_ptr->api;
}

/*!
 * \brief Retrieves the URL associated with this response.
 * \return The URL as a QUrl object.
 */
QUrl Response::url() const
{
    return d_ptr->netReply->request().url();
}

/*!
 * \brief Retrieves the network request used in this response.
 * \return The QNetworkRequest object.
 */
QNetworkRequest Response::networkRequest() const
{
    return d_ptr->netReply->request();
}

/*!
 * \brief Checks if the request is currently running.
 * \return \c true if the request is running, otherwise \c false.
 */
bool Response::isRunning() const
{
    return d_ptr->netReply->isRunning();
}

/*!
 * \brief Checks if the request has finished processing.
 * \return \c true if the request is finished, otherwise \c false.
 */
bool Response::isFinished() const
{
    return d_ptr->netReply->isFinished();
}

/*!
 * \brief Checks if the request was successful.
 * \return \c true if the request was successful, otherwise \c false.
 */
bool Response::isSuccess() const
{
    const QUrl url = d_ptr->netReply->url();
    if (url.scheme().contains("http"))
        return isHttpStatusSuccess();
    else
        return !hasNetworkError();
}

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
    return code >= 100 && code <= 500;
}

/*!
 * \brief Retrieves the HTTP status code of the response.
 * \return The HTTP status code.
 */
int Response::httpStatusCode() const
{
    return d_ptr->netReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}

/*!
 * \brief Retrieves the HTTP reason phrase of the response.
 * \return The HTTP reason phrase as a QString.
 */
QString Response::httpReasonPhrase() const
{
    return d_ptr->netReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
}

/*!
 * \brief Checks if the response contains a specific header.
 * \param name The name of the header to check.
 * \return \c true if the header is present, otherwise \c false.
 */
bool Response::hasHeader(const QByteArray &name) const
{
    return d_ptr->netReply->hasRawHeader(name);
}

/*!
 * \brief Retrieves the value of a specific header.
 * \param header The name of the header to retrieve.
 * \return The header value as a QByteArray.
 */
QByteArray Response::header(const QByteArray &header) const
{
    return d_ptr->netReply->rawHeader(header);
}

/*!
 * \brief Retrieves a list of all headers in the response.
 * \return A QByteArrayList containing the header names.
 */
QByteArrayList Response::headerList() const
{
    return d_ptr->netReply->rawHeaderList();
}

/*!
 * \brief Reads the response body as a JSON object.
 * \param error An optional pointer to a QJsonParseError to capture any parsing errors.
 * \return The response body as a QJsonObject.
 */
QJsonObject Response::readJsonObject(QJsonParseError *error)
{
    return readJson(error).toObject();
}

/*!
 * \brief Reads the response body as a JSON array.
 * \param error An optional pointer to a QJsonParseError to capture any parsing errors.
 * \return The response body as a QJsonArray.
 */
QJsonArray Response::readJsonArray(QJsonParseError *error)
{
    return readJson(error).toArray();
}

/*!
 * \brief Reads the response body as a JSON value.
 * \param error An optional pointer to a QJsonParseError to capture any parsing errors.
 * \return The response body as a QJsonValue.
 */
QJsonValue Response::readJson(QJsonParseError *error)
{
    const QJsonDocument doc = QJsonDocument::fromJson(readBody(), error);
    if (doc.isObject())
        return doc.object();
    else if (doc.isArray())
        return doc.array();
    else
        return QJsonValue();
}

/*!
 * \brief Reads the response body as a QString.
 * \return The response body as a QString.
 */
QString Response::readString()
{
    return QString::fromUtf8(readBody());
}

/*!
 * \brief Reads the raw response body as a QByteArray.
 * \return The response body as a QByteArray.
 */
QByteArray Response::readBody()
{
    if (d_ptr->netReply->bytesAvailable() > 0) {
        const QByteArray data = d_ptr->netReply->readAll();
        if (d_ptr->netReply->hasRawHeader("Content-Encoding"))
            return CompressionUtils::decompress(data, d_ptr->netReply->rawHeader("Content-Encoding"));
        else
            return data;
    } else
        return QByteArray();
}

/*!
 * \brief Checks if a network error occurred during the request.
 * \return \c true if a network error occurred, otherwise \c false.
 */
bool Response::hasNetworkError() const
{
    return d_ptr->netReply->error() != QNetworkReply::NoError;
}

/*!
 * \brief Retrieves the network error code.
 * \return The network error code.
 */
int Response::networkError() const
{
    return d_ptr->netReply->error();
}

/*!
 * \brief Retrieves a string description of the network error.
 * \return The network error description as a QString.
 */
QString Response::networkErrorString() const
{
    return d_ptr->netReply->errorString();
}

/*!
 * \brief Retrieves the QNetworkReply instance associated with this response.
 * \return A pointer to the QNetworkReply.
 */
QNetworkReply *Response::networkReply() const
{
    return d_ptr->netReply;
}

ResponsePrivate::ResponsePrivate(Response *q) :
    q_ptr(q),
    api(nullptr),
    netReply(nullptr)
{
}

}
