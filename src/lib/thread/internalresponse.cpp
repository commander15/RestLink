#include "internalresponse.h"
#include "internalresponse_p.h"

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace RestLink {

InternalResponse::InternalResponse(InternalResponsePrivate *d, Api *api)
    : Response(d, api)
{
}

InternalResponse::~InternalResponse()
{
}

ApiBase::Operation InternalResponse::operation() const
{
    RESTLINK_D(const InternalResponse);
    QMutexLocker locker(&d->mutex);
    return d->operation;
}

void InternalResponse::setOperation(ApiBase::Operation operation)
{
    RESTLINK_D(InternalResponse);
    QMutexLocker locker(&d->mutex);
    d->operation = operation;
}

bool InternalResponse::isFinished() const
{
    RESTLINK_D(const InternalResponse);
    QMutexLocker locker(&d->mutex);
    return d->finished;
}

int InternalResponse::httpStatusCode() const
{
    RESTLINK_D(const InternalResponse);
    QMutexLocker locker(&d->mutex);
    return d->httpStatusCode;
}

void InternalResponse::setHttpStatusCode(int code)
{
    RESTLINK_D(InternalResponse);
    QMutexLocker locker(&d->mutex);
    d->httpStatusCode = code;
}

bool InternalResponse::hasHeader(const QByteArray &name) const
{
    if (name.compare(QByteArrayLiteral("Content-Type"), Qt::CaseInsensitive) == 0)
        return true;

    RESTLINK_D(const InternalResponse);

    auto it = std::find_if(d->headers.begin(), d->headers.end(), [name](const Header &header) {
        return header.name() == name;
    });

    return (it != d->headers.end());
}

QByteArray InternalResponse::header(const QByteArray &name) const
{
    if (name.compare(QByteArrayLiteral("Content-Type"), Qt::CaseInsensitive) == 0)
        return QByteArrayLiteral("application/json");

    RESTLINK_D(const InternalResponse);

    auto it = std::find_if(d->headers.begin(), d->headers.end(), [name](const Header &header) {
        return header.name() == name;
    });

    return (it != d->headers.end() ? it->value().toByteArray() : QByteArray());
}

QByteArrayList InternalResponse::headerList() const
{
    RESTLINK_D(const InternalResponse);

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

void InternalResponse::setHeaders(const QList<Header> &headers)
{
    RESTLINK_D(InternalResponse);
    QMutexLocker locker(&d->mutex);
    d->headers = headers.toVector();
}

QJsonValue InternalResponse::readJson(QJsonParseError *error)
{
    RESTLINK_D(InternalResponse);

    QJsonValue value;
    d->mutex.lock();
    value.swap(d->data);
    d->mutex.unlock();
    return value;
}

QByteArray InternalResponse::readBody()
{
    RESTLINK_D(InternalResponse);

    QJsonValue value;
    d->mutex.lock();
    value.swap(d->data);
    d->mutex.unlock();

    QJsonDocument doc;
    if (value.isObject())
        doc.setObject(value.toObject());
    else if (value.isArray())
        doc.setArray(value.toArray());
    else {
        QJsonObject object;
        if (!value.isNull() && !value.isUndefined())
            object.insert("value", value);
        doc.setObject(object);
    }
    return doc.toJson(QJsonDocument::Compact);
}

void InternalResponse::setData(const QJsonValue &data)
{
    RESTLINK_D(InternalResponse);
    QMutexLocker locker(&d->mutex);
    d->data = data;
}

void InternalResponse::setData(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);

    RESTLINK_D(InternalResponse);
    QMutexLocker locker(&d->mutex);
    if (doc.isObject())
        d->data = doc.object();
    else if (doc.isArray())
        d->data = doc.array();
}

QNetworkRequest InternalResponse::networkRequest() const
{
    RESTLINK_D(const InternalResponse);
    return d->request;
}

void InternalResponse::setNetworkRequest(const QNetworkRequest &request)
{
    RESTLINK_D(InternalResponse);
    QMutexLocker locker(&d->mutex);
    d->request = request;
}

QNetworkReply *InternalResponse::networkReply() const
{
    return nullptr;
}

void InternalResponse::updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgress(bytesReceived, bytesTotal);
}

void InternalResponse::updateUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    emit uploadProgress(bytesSent, bytesTotal);
}

void InternalResponse::complete()
{
    RESTLINK_D(InternalResponse);
    d->mutex.lock();
    d->finished = true;
    d->mutex.unlock();

    emit finished();
}

void InternalResponse::ignoreSslErrors()
{
    // No-Op
}

void InternalResponse::abort()
{
    // No-Op
}

InternalResponsePrivate::InternalResponsePrivate(InternalResponse *q)
    : ResponsePrivate(q)
    , operation(Api::UnknownOperation)
    , httpStatusCode(0)
    , finished(false)
{
}

} // namespace RestLink
