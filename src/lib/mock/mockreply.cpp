#include "mockreply.h"

#include <QtCore/qtimer.h>
#include <QtCore/qbuffer.h>

namespace RestLink {

MockReply::MockReply(const QNetworkRequest &request, QObject *parent)
    : QNetworkReply{parent}
    , m_buffer(nullptr)
{
    setRequest(request);

    QTimer::singleShot(0, this, &QNetworkReply::finished);
}

qint64 MockReply::pos() const
{
    return m_buffer->pos();
}

qint64 MockReply::size() const
{
    return m_buffer->size();
}

bool MockReply::seek(qint64 pos)
{
    return m_buffer->seek(pos);
}

bool MockReply::atEnd() const
{
    return m_buffer->atEnd();
}

bool MockReply::reset()
{
    return m_buffer->reset();
}

qint64 MockReply::bytesAvailable() const
{
    return m_buffer->bytesAvailable();
}

qint64 MockReply::bytesToWrite() const
{
    return m_buffer->bytesToWrite();
}

MockReply *MockReply::create(const QNetworkRequest &request, const MockData &data, QObject *parent)
{
    MockReply *reply = new MockReply(request, parent);
    reply->setMockData(data);
    return reply;
}

qint64 MockReply::readData(char *data, qint64 maxlen)
{
    qint64 remain = maxlen;
    qint64 bytesRead = 0;

    while (m_buffer->bytesAvailable() > 0 && remain > 0) {
        char c;
        if (m_buffer->getChar(&c)) {
            data[bytesRead] = c;  // Append character
            ++bytesRead;
            --remain;
        }
    }

    return bytesRead;  // Return actual bytes read
}

qint64 MockReply::writeData(const char *data, qint64 len)
{
    qint64 written = 0;
    for (qint64 i = 0; i < len; ++i) {
        if (m_buffer->putChar(data[i])) {
            ++written;
        }
    }
    return written;  // Return number of bytes written
}

void MockReply::setMockData(const MockData &data)
{
    for (const Header &header : data.headers)
        setRawHeader(header.name().toLatin1(), header.value().toByteArray());

    if (data.body.isDevice())
        m_buffer = data.body.device();
    else if (data.body.isData()) {
        QBuffer *buffer = new QBuffer(this);
        buffer->setData(data.body.data());
        buffer->open(QIODevice::ReadOnly);
        m_buffer = buffer;
    }
}

void MockReply::abort()
{
    // No-Op
}

} // namespace RestLink
