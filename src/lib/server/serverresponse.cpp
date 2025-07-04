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

ServerResponse::~ServerResponse()
{
}

AbstractRequestHandler::Method ServerResponse::method() const
{
    RESTLINK_D(const ServerResponse);
    QReadLocker locker(&d->lock);
    return d->method;
}

void ServerResponse::setMethod(AbstractRequestHandler::Method method)
{
    RESTLINK_D(ServerResponse);
    QWriteLocker locker(&d->lock);
    d->method = method;
}

bool ServerResponse::isFinished() const
{
    RESTLINK_D(const ServerResponse);
    QReadLocker locker(&d->lock);
    return d->finished;
}

int ServerResponse::httpStatusCode() const
{
    RESTLINK_D(const ServerResponse);
    QReadLocker locker(&d->lock);
    return d->httpStatusCode;
}

void ServerResponse::setHttpStatusCode(int code)
{
    RESTLINK_D(ServerResponse);
    QWriteLocker locker(&d->lock);
    d->httpStatusCode = code;
}

bool ServerResponse::hasHeader(const QString &name) const
{
    RESTLINK_D(const ServerResponse);
    QReadLocker locker(&d->lock);
    return d->headers.contains(name) || d->body.headers().contains(name);
}

QString ServerResponse::header(const QString &name) const
{
    RESTLINK_D(const ServerResponse);
    QReadLocker locker(&d->lock);

    if (d->headers.contains(name))
        return d->headers.parameter(name).value().toByteArray();

    const HeaderList bodyHeaders = d->body.headers();
    if (bodyHeaders.contains(name))
        return bodyHeaders.value(name).toByteArray();

    return QByteArray();
}

QStringList ServerResponse::headerList() const
{
    RESTLINK_D(const ServerResponse);
    QReadLocker locker(&d->lock);
    QStringList parameterNames = d->headers.parameterNames() + d->body.headers().parameterNames();
    parameterNames.removeDuplicates();
    return parameterNames;
}

void ServerResponse::setHeaders(const QList<Header> &headers)
{
    RESTLINK_D(ServerResponse);
    QWriteLocker locker(&d->lock);
    d->headers = headers.toVector();
}

QJsonObject ServerResponse::readJsonObject(QJsonParseError *error)
{
    RESTLINK_D(ServerResponse);
    QWriteLocker locker(&d->lock);
    return (d->body.hasJsonObject() ? d->readBody().jsonObject() : QJsonObject());
}

QJsonArray ServerResponse::readJsonArray(QJsonParseError *error)
{
    RESTLINK_D(ServerResponse);
    QWriteLocker locker(&d->lock);
    return (d->body.hasJsonArray() ? d->readBody().jsonArray() : QJsonArray());
}

QJsonValue ServerResponse::readJson(QJsonParseError *error)
{
    RESTLINK_D(ServerResponse);
    QWriteLocker locker(&d->lock);

    if (d->body.hasJsonObject())
        return d->readBody().jsonObject();

    if (d->body.hasJsonArray())
        return d->readBody().jsonArray();

    return QJsonValue();
}

QString ServerResponse::readString()
{
    RESTLINK_D(ServerResponse);
    QWriteLocker locker(&d->lock);
    return d->readBody().toString();
}

QByteArray ServerResponse::readBody()
{
    RESTLINK_D(ServerResponse);
    QWriteLocker locker(&d->lock);
    return d->readBody().toByteArray();
}

void ServerResponse::setBody(const Body &body)
{
    RESTLINK_D(ServerResponse);
    QWriteLocker locker(&d->lock);
    d->body = body;
}

QNetworkRequest ServerResponse::networkRequest() const
{
    RESTLINK_D(const ServerResponse);
    QReadLocker locker(&d->lock);
    return d->networkRequest;
}

void ServerResponse::setNetworkRequest(const QNetworkRequest &request)
{
    RESTLINK_D(ServerResponse);
    QWriteLocker locker(&d->lock);
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
    d->lock.lockForWrite();
    d->finished = true;
    d->lock.unlock();

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
    , atEnd(false)
    , server(nullptr)
{
}

ServerResponsePrivate::~ServerResponsePrivate()
{
}

Body ServerResponsePrivate::readBody()
{
    if (atEnd) {
        return Body();
    } else {
        atEnd = true;
        return body;
    }
}

} // namespace RestLink
