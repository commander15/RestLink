#include "body.h"

#include <RestLink/debug.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qfile.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qmimedatabase.h>

namespace RestLink {

static QMimeDatabase mimeDb;

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
    : Body(file, file->size(), mimeDb.mimeTypeForFile(file->fileName()).name().toLatin1())
{
}

Body::Body(QIODevice *device, qint64 size, const QByteArray &contentType)
    : m_multiPart(nullptr)
    , m_device(device)
    , m_length(size >= 0 ? size : (device->isSequential() ? -1 : device->size()))
{
    if (!contentType.isEmpty())
        m_type = contentType;
    else
        m_type = mimeDb.mimeTypeForData(device).name().toLatin1();
}

Body::Body(const QString &text, const QByteArray &contentType)
    : Body(text.toUtf8(), contentType)
{
}

Body::Body(const QByteArray &data, const QByteArray &contentType)
    : Body(data.constData(), data.size(), contentType)
{
}

Body::Body(const char *data, int size, const QByteArray &contentType)
    : m_multiPart(nullptr)
    , m_device(nullptr)
{
    QByteArray bodyData(data, size);

    m_data = bodyData;
    m_length = bodyData.size();
    if (!contentType.isEmpty())
        m_type = contentType;
    else
        m_type = mimeDb.mimeTypeForData(bodyData).name().toLatin1();
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

Body &Body::operator=(const Body &other)
{
    if (this != &other) {
        m_multiPart = other.m_multiPart;
        m_device = other.m_device;
        m_data = other.m_data;
        m_type = other.m_type;
        m_length = other.m_length;
        m_headers = other.m_headers;
    }

    return *this;
}

Body &Body::operator=(Body &&other)
{
    if (this != &other) {
        m_multiPart = std::move(other.m_multiPart);
        m_device = std::move(other.m_device);
        m_data = std::move(other.m_data);
        m_type = std::move(other.m_type);
        m_length = std::move(other.m_length);
        m_headers = std::move(other.m_headers);
    }

    return *this;
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
        type = data.userType();

    // Generate headers based on the type of the data provided
    HeaderList headers;

    // You can add your header-generating logic here

    return headers;
}

}
