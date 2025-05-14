#include "responsebase.h"

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonvalue.h>

namespace RestLink {

/*!
 * \class RestLink::ResponseBase
 * \brief An abstract base class for handling response data streams.
 *
 * This class extends QIODevice to provide convenient methods for reading
 * different types of content (JSON objects, arrays, raw bytes, strings)
 * from a response device. It is intended to serve as a flexible interface
 * for response handling in the RestLink framework.
 *
 * It supports streaming and can wrap any QIODevice to perform actual data access.
 */

ResponseBase::ResponseBase(QObject *parent)
    : QIODevice(parent)
    , m_device(nullptr)
{
}

ResponseBase::~ResponseBase()
{
}

bool ResponseBase::isSequential() const
{
    return m_device->isSequential();
}

bool ResponseBase::open(OpenMode mode)
{
    return m_device->open(mode);
}

void ResponseBase::close()
{
    m_device->close();
}

qint64 ResponseBase::pos() const
{
    return m_device->pos();
}

qint64 ResponseBase::size() const
{
    return m_device->size();
}

bool ResponseBase::seek(qint64 pos)
{
    return m_device->seek(pos);
}

bool ResponseBase::atEnd() const
{
    return m_device->atEnd();
}

bool ResponseBase::reset()
{
    return m_device->reset();
}

qint64 ResponseBase::bytesAvailable() const
{
    return m_device->bytesAvailable();
}

qint64 ResponseBase::bytesToWrite() const
{
    return m_device->bytesToWrite();
}

bool ResponseBase::canReadLine() const
{
    return m_device->canReadLine();
}

bool ResponseBase::waitForReadyRead(int msecs)
{
    return m_device->waitForReadyRead(msecs);
}

bool ResponseBase::waitForBytesWritten(int msecs)
{
    return m_device->waitForBytesWritten(msecs);
}

/*!
 * \brief Reads the response body as a JSON object.
 * \param error An optional pointer to a QJsonParseError to capture any parsing errors.
 * \return The response body as a QJsonObject.
 */
QJsonObject ResponseBase::readJsonObject(QJsonParseError *error)
{
    return readJson(error).toObject();
}

/*!
 * \brief Reads the response body as a JSON array.
 * \param error An optional pointer to a QJsonParseError to capture any parsing errors.
 * \return The response body as a QJsonArray.
 */
QJsonArray ResponseBase::readJsonArray(QJsonParseError *error)
{
    return readJson(error).toArray();
}

/*!
 * \brief Reads the response body as a JSON value.
 * \param error An optional pointer to a QJsonParseError to capture any parsing errors.
 * \return The response body as a QJsonValue.
 */
QJsonValue ResponseBase::readJson(QJsonParseError *error)
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
QString ResponseBase::readString()
{
    return QString::fromUtf8(readBody());
}

/*!
 * \brief Reads the raw response body as a QByteArray.
 * \return The response body as a QByteArray.
 */
QByteArray ResponseBase::readBody()
{
    return m_device->readAll();
}

QIODevice *ResponseBase::responseDevice() const
{
    return m_device;
}

qint64 ResponseBase::readData(char *data, qint64 maxlen)
{
    return m_device->read(data, maxlen);
}

qint64 ResponseBase::readLineData(char *data, qint64 maxlen)
{
    return m_device->readLine(data, maxlen);
}

qint64 ResponseBase::skipData(qint64 maxSize)
{
    return m_device->skip(maxSize);
}

qint64 ResponseBase::writeData(const char *data, qint64 len)
{
    return m_device->write(data, len);
}

void ResponseBase::setResponseDevice(QIODevice *device)
{
    m_device = device;

    connect(device, &QIODevice::readyRead, this, &QIODevice::readyRead);
    connect(device, &QIODevice::channelReadyRead, this, &QIODevice::channelReadyRead);
    connect(device, &QIODevice::bytesWritten, this, &QIODevice::bytesWritten);
    connect(device, &QIODevice::channelBytesWritten, this, &QIODevice::channelBytesWritten);
    connect(device, &QIODevice::aboutToClose, this, &QIODevice::aboutToClose);
    connect(device, &QIODevice::readChannelFinished, this, &QIODevice::readChannelFinished);
}

} // namespace RestLink
