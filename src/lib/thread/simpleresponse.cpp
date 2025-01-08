#include "simpleresponse.h"
#include "simpleresponse_p.h"

namespace RestLink {

SimpleResponse::SimpleResponse(Api *api)
    : Response(new SimpleResponsePrivate(this), api)
{
}

bool SimpleResponse::hasHeader(const QByteArray &name) const
{
    return headerList().contains(name);
}

void SimpleResponse::setHeaders(const QList<Header> &headers)
{
    RESTLINK_D(SimpleResponse);
    QMutexLocker locker(&d->mutex);
    d->headers = headers.toVector();
}

QByteArray SimpleResponse::readBody()
{
    RESTLINK_D(SimpleResponse);
    QMutexLocker locker(&d->mutex);
    return d->data;
}

void SimpleResponse::setData(const QByteArray &data)
{
    RESTLINK_D(SimpleResponse);
    d->mutex.lock();
    d->data = data;
    d->mutex.unlock();

    emit finished();
}

SimpleResponsePrivate::SimpleResponsePrivate(SimpleResponse *q)
    : ResponsePrivate(q)
{
}

} // namespace RestLink
