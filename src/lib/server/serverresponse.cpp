#include "serverresponse.h"
#include "serverresponse_p.h"

#include <RestLink/server.h>

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qtimer.h>

namespace RestLink {

ServerResponse::ServerResponse(Server *server)
    : Response(new ServerResponsePrivate(this), server)
{
    RESTLINK_D(ServerResponse);
    d->server = server;
}

RequestHandler::Method ServerResponse::method() const
{
    RESTLINK_D(const ServerResponse);
    QMutexLocker locker(&d->mutex);
    return d->method;
}

void ServerResponse::setMethod(RequestHandler::Method method)
{
    RESTLINK_D(ServerResponse);
    QMutexLocker locker(&d->mutex);
    d->method = method;
}

bool ServerResponse::isFinished() const
{
    RESTLINK_D(const ServerResponse);
    QMutexLocker locker(&d->mutex);
    return d->finished;
}

int ServerResponse::httpStatusCode() const
{
    RESTLINK_D(const ServerResponse);
    QMutexLocker locker(&d->mutex);
    return d->httpStatusCode;
}

void ServerResponse::setHttpStatusCode(int code)
{
    RESTLINK_D(ServerResponse);
    QMutexLocker locker(&d->mutex);
    d->httpStatusCode = code;
}

bool ServerResponse::hasHeader(const QByteArray &name) const
{
    if (name.compare(QByteArrayLiteral("Content-Type"), Qt::CaseInsensitive) == 0)
        return true;

    RESTLINK_D(const ServerResponse);

    auto it = std::find_if(d->headers.begin(), d->headers.end(), [name](const Header &header) {
        return header.name() == name;
    });

    return (it != d->headers.end());
}

QByteArray ServerResponse::header(const QByteArray &name) const
{
    if (name.compare(QByteArrayLiteral("Content-Type"), Qt::CaseInsensitive) == 0)
        return QByteArrayLiteral("application/json");

    RESTLINK_D(const ServerResponse);

    auto it = std::find_if(d->headers.begin(), d->headers.end(), [name](const Header &header) {
        return header.name() == name;
    });

    return (it != d->headers.end() ? it->value().toByteArray() : QByteArray());
}

QByteArrayList ServerResponse::headerList() const
{
    RESTLINK_D(const ServerResponse);

#if QT_VERSION_MAJOR >= 6
    QByteArrayList names(d->headers.size());
    std::transform(d->headers.begin(), d->headers.end(), names.begin(), [](const Header &header) {
        return header.name().toUtf8();
    });
#else
    QByteArrayList names;
    for (const Header &header : d->headers)
        names.append(header.name().toUtf8());
#endif

    return names;
}

void ServerResponse::setHeaders(const QList<Header> &headers)
{
    RESTLINK_D(ServerResponse);
    QMutexLocker locker(&d->mutex);
    d->headers = headers.toVector();
}

QJsonObject ServerResponse::readJsonObject(QJsonParseError *error)
{
    RESTLINK_D(ServerResponse);
    QMutexLocker locker(&d->mutex);
    return (d->body.hasJsonObject() ? d->body.jsonObject() : QJsonObject());
}

QJsonArray ServerResponse::readJsonArray(QJsonParseError *error)
{
    RESTLINK_D(ServerResponse);
    QMutexLocker locker(&d->mutex);
    return (d->body.hasJsonArray() ? d->body.jsonArray() : QJsonArray());
}

QJsonValue ServerResponse::readJson(QJsonParseError *error)
{
    RESTLINK_D(ServerResponse);
    QMutexLocker locker(&d->mutex);

    if (d->body.hasJsonObject())
        return d->body.jsonObject();

    if (d->body.hasJsonArray())
        return d->body.jsonArray();

    return QJsonValue();
}

QString ServerResponse::readString()
{
    RESTLINK_D(ServerResponse);
    QMutexLocker locker(&d->mutex);
    return d->body.toString();
}

QByteArray ServerResponse::readBody()
{
    RESTLINK_D(ServerResponse);
    QMutexLocker locker(&d->mutex);
    return d->body.data();
    return d->body.toByteArray();
}

void ServerResponse::setBody(const Body &body)
{
    RESTLINK_D(ServerResponse);
    QMutexLocker locker(&d->mutex);
    d->body = body;

    const QList<Header> headers = body.headers();
    for (const Header &header : headers)
        if (!d->headers.contains(header.name()))
            d->headers.append(header);
}

QNetworkRequest ServerResponse::networkRequest() const
{
    RESTLINK_D(const ServerResponse);
    return d->networkRequest;
}

void ServerResponse::setNetworkRequest(const QNetworkRequest &request)
{
    RESTLINK_D(ServerResponse);
    QMutexLocker locker(&d->mutex);
    d->networkRequest = request;
}

QNetworkReply *ServerResponse::networkReply() const
{
    return nullptr;
}

Server *ServerResponse::server() const
{
    RESTLINK_D(const ServerResponse);
    return d->server;
}

void ServerResponse::updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgress(bytesReceived, bytesTotal);
}

void ServerResponse::updateUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    emit uploadProgress(bytesSent, bytesTotal);
}

void ServerResponse::complete()
{
    RESTLINK_D(ServerResponse);
    d->mutex.lock();
    d->finished = true;
    d->mutex.unlock();

    emit finished();
}

void ServerResponse::ignoreSslErrors()
{
    // No-Op
}

void ServerResponse::abort()
{
    // No-Op
}

ServerResponsePrivate::ServerResponsePrivate(ServerResponse *q)
    : ResponsePrivate(q)
    , method(RequestHandler::UnknownMethod)
    , httpStatusCode(200)
    , finished(false)
    , server(nullptr)
{
}

} // namespace RestLink
