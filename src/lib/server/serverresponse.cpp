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

AbstractRequestHandler::Method ServerResponse::method() const
{
    RESTLINK_D(const ServerResponse);
    QMutexLocker locker(&d->mutex);
    return d->method;
}

void ServerResponse::setMethod(AbstractRequestHandler::Method method)
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
    RESTLINK_D(const ServerResponse);

    auto it = std::find_if(d->headers.begin(), d->headers.end(), [name](const Header &header) {
        return header.name() == name;
    });

    if (it != d->headers.end())
        return it->value().toByteArray();

    const HeaderList headers = d->body.headers();
    it = std::find_if(headers.begin(), headers.end(), [name](const Header &header) {
        return header.name() == name;
    });

    if (it != d->headers.end())
        return it->value().toByteArray();

    return QByteArray();
}

QByteArrayList ServerResponse::headerList() const
{
    RESTLINK_D(const ServerResponse);

    const HeaderList headers = d->headers + d->body.headers();
    QByteArrayList names(headers.size());
    std::transform(headers.begin(), headers.end(), names.begin(), [](const Header &header) {
        return header.name().toUtf8();
    });

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
    return d->body.toByteArray();
}

void ServerResponse::setBody(const Body &body)
{
    RESTLINK_D(ServerResponse);
    QMutexLocker locker(&d->mutex);
    d->body = body;
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
    , method(AbstractRequestHandler::UnknownMethod)
    , httpStatusCode(200)
    , finished(false)
    , server(nullptr)
{
}

} // namespace RestLink
