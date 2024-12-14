#include "apireply.h"
#include "apireply_p.h"

#include <RestLink/debug.h>
#include <RestLink/api.h>
#include <RestLink/compressionutils.h>

#include <QtNetwork/qnetworkreply.h>

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace RestLink {

/*!
 * \class RestLink::ApiReply
 * \brief Represents a reply to an API request in the RestLink framework.
 *
 * The ApiReply class encapsulates the response from an API request, providing methods for accessing
 * various aspects of the response such as status, headers, body, and progress. It also provides
 * signals for progress updates, completion, and error handling.
 *
 * The ApiReply object is associated with a specific API request and provides functionality to
 * handle download/upload progress, SSL errors, network errors, and HTTP status codes. It also
 * supports reading the response body as JSON or raw data and provides the option to abort or ignore
 * specific errors during the process.
 *
 * \property QString endpoint
 * \brief The endpoint of the API request.
 *
 * \property RestLink::Api::Operation operation
 * \brief The HTTP operation (e.g., GET, POST) associated with this reply.
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
 * \brief Protected constructor for ApiReply.
 * \param api A pointer to the Api instance that initiated the request.
 */
ApiReply::ApiReply(Api *api) :
    QObject(api),
    d(new ApiReplyPrivate(this))
{
    d->api = api;
}

/*!
 * \brief Protected constructor for ApiReply with private data.
 * \param dd A pointer to the private data object.
 * \param api A pointer to the Api instance that initiated the request.
 */
ApiReply::ApiReply(ApiReplyPrivate *dd, Api *api) :
    QObject(api),
    d(dd)
{
    d->api = api;
}

/*!
 * \brief Destructor for ApiReply.
 *
 * Cleans up resources held by the ApiReply object.
 */
ApiReply::~ApiReply()
{
}

/*!
 * \brief Retrieves the API endpoint associated with this reply.
 * \return The endpoint as a QString.
 */
QString ApiReply::endpoint() const
{
    return d->apiRequest.endpoint();
}

/*!
 * \brief Retrieves the API request associated with this reply.
 * \return The ApiRequest object.
 */
ApiRequest ApiReply::apiRequest() const
{
    return d->apiRequest;
}

/*!
 * \brief Retrieves the HTTP operation type for this reply.
 * \return The HTTP operation as an Api::Operation.
 */
ApiBase::Operation ApiReply::operation() const
{
    switch (d->netReply->operation()) {
    case QNetworkAccessManager::GetOperation:
        return Api::GetOperation;

    case QNetworkAccessManager::PostOperation:
        return Api::PostOperation;

    case QNetworkAccessManager::PutOperation:
        return Api::PutOperation;

    case QNetworkAccessManager::DeleteOperation:
        return Api::DeleteOperation;

    case QNetworkAccessManager::CustomOperation:
    default:
        return Api::PatchOperation;
    }
}

/*!
 * \brief Retrieves the API instance associated with this reply.
 * \return A pointer to the Api object.
 */
Api *ApiReply::api() const
{
    return d->api;
}

/*!
 * \brief Retrieves the URL associated with this reply.
 * \return The URL as a QUrl object.
 */
QUrl ApiReply::url() const
{
    return d->netReply->request().url();
}

/*!
 * \brief Retrieves the network request used in this reply.
 * \return The QNetworkRequest object.
 */
QNetworkRequest ApiReply::networkRequest() const
{
    return d->netReply->request();
}

/*!
 * \brief Checks if the HTTP status code indicates success.
 * \return \c true if the HTTP status code indicates success, otherwise \c false.
 */
bool ApiReply::isHttpStatusSuccess() const
{
    const int code = httpStatusCode();
    return (code >= 200 && code <= 299);
}

/*!
 * \brief Retrieves the HTTP status code of the response.
 * \return The HTTP status code.
 */
int ApiReply::httpStatusCode() const
{
    return d->netReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}

/*!
 * \brief Retrieves the HTTP reason phrase of the response.
 * \return The HTTP reason phrase as a QString.
 */
QString ApiReply::httpReasonPhrase() const
{
    return d->netReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
}

/*!
 * \brief Checks if the response contains a specific header.
 * \param name The name of the header to check.
 * \return \c true if the header is present, otherwise \c false.
 */
bool ApiReply::hasHeader(const QByteArray &name) const
{
    return d->netReply->hasRawHeader(name);
}

/*!
 * \brief Retrieves the value of a specific header.
 * \param header The name of the header to retrieve.
 * \return The header value as a QByteArray.
 */
QByteArray ApiReply::header(const QByteArray &header) const
{
    return d->netReply->rawHeader(header);
}

/*!
 * \brief Retrieves a list of all headers in the response.
 * \return A QByteArrayList containing the header names.
 */
QByteArrayList ApiReply::headerList() const
{
    return d->netReply->rawHeaderList();
}

/*!
 * \brief Checks if the request is currently running.
 * \return \c true if the request is running, otherwise \c false.
 */
bool ApiReply::isRunning() const
{
    return d->netReply->isRunning();
}

/*!
 * \brief Checks if the request has finished processing.
 * \return \c true if the request is finished, otherwise \c false.
 */
bool ApiReply::isFinished() const
{
    return d->netReply->isFinished();
}

/*!
 * \brief Aborts the ongoing request.
 */
void ApiReply::abort()
{
    d->netReply->abort();
}

/*!
 * \brief Checks if the request was successful.
 * \return \c true if the request was successful, otherwise \c false.
 */
bool ApiReply::isSuccess() const
{
    if (isFinished())
        return !hasNetworkError() && isHttpStatusSuccess();
    else
        return false;
}

/*!
 * \brief Reads the response body as a JSON object.
 * \param error An optional pointer to a QJsonParseError to capture any parsing errors.
 * \return The response body as a QJsonObject.
 */
QJsonObject ApiReply::readJsonObject(QJsonParseError *error)
{
    return readJson(error).toObject();
}

/*!
 * \brief Reads the response body as a JSON array.
 * \param error An optional pointer to a QJsonParseError to capture any parsing errors.
 * \return The response body as a QJsonArray.
 */
QJsonArray ApiReply::readJsonArray(QJsonParseError *error)
{
    return readJson(error).toArray();
}

/*!
 * \brief Reads the response body as a JSON value.
 * \param error An optional pointer to a QJsonParseError to capture any parsing errors.
 * \return The response body as a QJsonValue.
 */
QJsonValue ApiReply::readJson(QJsonParseError *error)
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
QString ApiReply::readString()
{
    return QString::fromUtf8(readBody());
}

/*!
 * \brief Reads the raw response body as a QByteArray.
 * \return The response body as a QByteArray.
 */
QByteArray ApiReply::readBody()
{
    if (d->netReply->bytesAvailable() > 0) {
        const QByteArray data = d->netReply->readAll();
        if (d->netReply->hasRawHeader("Content-Encoding"))
            return CompressionUtils::decompress(data, d->netReply->rawHeader("Content-Encoding"));
        else
            return data;
    } else
        return QByteArray();
}

/*!
 * \brief Ignores specific SSL errors.
 * \param errors The list of SSL errors to ignore.
 */
void ApiReply::ignoreSslErros(const QList<QSslError> &errors)
{
    d->netReply->ignoreSslErrors(errors);
}

/*!
 * \brief Checks if a network error occurred during the request.
 * \return \c true if a network error occurred, otherwise \c false.
 */
bool ApiReply::hasNetworkError() const
{
    return d->netReply->error() != QNetworkReply::NoError;
}

/*!
 * \brief Retrieves the network error code.
 * \return The network error code.
 */
int ApiReply::networkError() const
{
    return d->netReply->error();
}

/*!
 * \brief Retrieves a string description of the network error.
 * \return The network error description as a QString.
 */
QString ApiReply::networkErrorString() const
{
    return d->netReply->errorString();
}

/*!
 * \brief Retrieves the QNetworkReply instance associated with this reply.
 * \return A pointer to the QNetworkReply.
 */
QNetworkReply *ApiReply::networkReply() const
{
    return d->netReply;
}

/*!
 * \brief Sets the ApiRequest for this reply.
 * \param request The ApiRequest to associate with this reply.
 */
void ApiReply::setApiRequest(const ApiRequest &request)
{
    d->apiRequest = request;
}

/*!
 * \brief Sets the QNetworkReply for this reply.
 * \param reply The QNetworkReply to associate with this reply.
 */
void ApiReply::setNetworkReply(QNetworkReply *reply)
{
    d->netReply = reply;
    d->netReply->setParent(this);

    connect(reply, &QNetworkReply::readyRead, this, &ApiReply::readyRead);
    connect(reply, &QNetworkReply::downloadProgress, this, &ApiReply::downloadProgress);
    connect(reply, &QNetworkReply::uploadProgress, this, &ApiReply::uploadProgress);
    connect(reply, &QNetworkReply::sslErrors, this, &ApiReply::sslErrorsOccured);
    connect(reply, &QNetworkReply::errorOccurred, this, &ApiReply::networkErrorOccured);
    connect(reply, &QNetworkReply::finished, this, &ApiReply::finished);
}

ApiReplyPrivate::ApiReplyPrivate(ApiReply *qq) :
    q(qq),
    api(nullptr),
    netReply(nullptr)
{
}

}
