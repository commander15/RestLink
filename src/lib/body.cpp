#include "body.h"

#include <RestLink/debug.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qfile.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace RestLink {

File::File(const char *fileName)
    : m_fileName(fileName)
{
}

File::File(const QString &fileName)
    : m_fileName(fileName)
{
}

QFile* File::open() const
{
    QFile* file = new QFile(m_fileName, qApp);
    if (file->open(QIODevice::ReadOnly)) {
        return file;
    } else {
        delete file;
        return nullptr;
    }
}

Body::Body()
    : m_multiPart(nullptr)
    , m_device(nullptr)
    , m_length(-1)
{
}

Body::Body(QHttpMultiPart *multiPart)
    : m_multiPart(multiPart)
    , m_device(nullptr)
    , m_length(-1)
{
}

Body::Body(const File &file)
    : Body(file.open())
{
    // Assuming file will handle device opening and reading data
}

Body::Body(QFile *file)
    : Body(file, file->size())
{
}

Body::Body(QIODevice *device, qint64 size, const QByteArray &contentType)
    : m_multiPart(nullptr)
    , m_device(device)
    , m_type(contentType)
    , m_length(size >= 0 ? size : (device->isSequential() ? -1 : device->size()))
{
}

Body::Body(const char *data, int size, const QByteArray &contentType)
    : m_multiPart(nullptr)
    , m_device(nullptr)
    , m_data(QByteArray(data, size))
    , m_type(contentType)
    , m_length(size)
{
}

Body::Body(const QByteArray &data, const QByteArray &contentType)
    : m_multiPart(nullptr)
    , m_device(nullptr)
    , m_data(data)
    , m_type(contentType)
    , m_length(data.size())
{
}

Body::Body(const QString &text, const QByteArray &contentType)
    : m_multiPart(nullptr)
    , m_device(nullptr)
    , m_data(text.toUtf8())
    , m_type(contentType)
    , m_length(text.size())
{
}

Body::Body(const QJsonObject &object)
    : Body(QJsonDocument(object))
{
}

Body::Body(const QJsonArray &array)
    : Body(QJsonDocument(array))
{
}

Body::Body(const QJsonDocument &doc)
    : m_multiPart(nullptr)
    , m_device(nullptr)
    , m_type(QByteArrayLiteral("application/json"))
{
    const QByteArray data = doc.toJson(QJsonDocument::Compact);
    m_data = data;
    m_length = data.size();
}

Body::Body(const QVariant &value, int type, const QByteArray &contentType)
    : m_multiPart(nullptr)
    , m_device(nullptr)
    , m_data(generateData(value, type))
    , m_type(contentType)
    , m_length(-1)
{
}

Body::~Body()
{
}

bool Body::isMultiPart() const
{
    return m_multiPart;
}

QHttpMultiPart* Body::multiPart() const
{
    return m_multiPart;
}

bool Body::isDevice() const
{
    return m_device;
}

QIODevice* Body::device() const
{
    return m_device;
}

bool Body::isData() const
{
    return !m_data.isNull();
}

QByteArray Body::data() const
{
    return m_data.toByteArray();
}

HeaderList Body::headers() const
{
    HeaderList headers;

    if (!m_data.isNull())
        headers.append(generateHeaders(m_data));

    if (!m_type.isEmpty())
        headers.append(Header("Content-Type", m_type));

    if (m_length > 0)
        headers.append(Header("Content-Length", m_length));

    return headers + m_headers;
}

void Body::setHeaders(const HeaderList &headers)
{
    m_headers = headers;
}

QByteArray Body::generateData(const QVariant &data, int type)
{
    Q_UNUSED(type);
    return data.toByteArray();
}

QList<Header> Body::generateHeaders(const QVariant &data, int type)
{
    if (type == -1)
        type = data.metaType().id();

    // Generate headers based on the type of the data provided
    HeaderList headers;

    // You can add your header-generating logic here

    return headers;
}

}
