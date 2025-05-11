#include "body.h"
#include "body_p.h"

#include <RestLink/debug.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qfile.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qiodevice.h>

#include <QtNetwork/qhttpmultipart.h>

#define TYPE_PLAIN_TEXT "text/plain"
#define TYPE_JSON       "application/json"

namespace RestLink {

Body::Body()
    : d_ptr(new BodyData())
{
}

Body::Body(const char *data, int size, const QByteArray &contentType)
    : Body(QVariant(QByteArray(data, size)), contentType)
{
}

Body::Body(const QByteArray &data)
    : Body(QVariant(data), TYPE_PLAIN_TEXT)
{
}

Body::Body(const QByteArray &data, const QByteArray &contentType)
    : Body(QVariant(data), contentType)
{
}

Body::Body(const QString &text)
    : Body(QVariant(text), TYPE_PLAIN_TEXT)
{
}

Body::Body(const QString &text, const QByteArray &contentType)
    : Body(QVariant(text), contentType)
{
}

Body::Body(const QJsonValue &value)
    : Body(value.toVariant(), TYPE_JSON)
{
}

Body::Body(const QJsonObject &object)
    : Body(QVariant(object), TYPE_JSON)
{
}

Body::Body(const QJsonArray &array)
    : Body(QVariant(array), TYPE_JSON)
{
}

Body::Body(const QJsonDocument &doc)
    : Body(doc.isObject() ? QVariant::fromValue(doc.object()) : QVariant::fromValue(doc.array()), TYPE_JSON)
{
}

Body::Body(const QVariant &value, const QByteArray &contentType)
    : d_ptr(new BodyData())
{
    d_ptr->object = value;
    d_ptr->contentType = contentType;

    const QMetaType metaType = value.metaType();

    if (QMetaType::canConvert(metaType, QMetaType::fromType<QString>())) {
        d_ptr->objectType = Type::PlainText;
        return;
    }

    const QString typeName(metaType.name());
    if (typeName.startsWith("QJson")) {
        d_ptr->objectType = Type::JsonData;
        return;
    }
}

Body::Body(const File &file)
    : Body(file.open())
{
}

Body::Body(QFile *file)
    : Body(file, file->size(), s_mimeDatabase.mimeTypeForFile(file->fileName()).name().toLatin1())
{
}

Body::Body(QIODevice *device, qint64 size, const QByteArray &contentType)
    : d_ptr(new BodyData())
{
    d_ptr->object = QVariant::fromValue(device);
    d_ptr->objectType = Type::IODevice;
    d_ptr->contentType = contentType;
}

Body::Body(QHttpMultiPart *multiPart)
    : d_ptr(new BodyData())
{
    d_ptr->object = QVariant::fromValue(multiPart);
    d_ptr->objectType = Type::HttpMultiPart;
}

Body::Body(const Body &other) = default;

Body::Body(Body &&other) = default;

Body::~Body()
{
}

Body &Body::operator=(const Body &other)
{
    if (this != &other) {
        d_ptr = other.d_ptr;
    }

    return *this;
}

Body &Body::operator=(Body &&other)
{
    if (this != &other) {
        d_ptr.swap(other.d_ptr);
    }

    return *this;
}

bool Body::hasPlainText() const
{
    return d_ptr->objectType == Type::PlainText;
}

QByteArray Body::toByteArray() const
{
    if (d_ptr->objectType == Type::PlainText)
        return d_ptr->object.toByteArray();
    else
        return QByteArray();
}

QString Body::toString() const
{
    if (d_ptr->objectType == Type::PlainText)
        return d_ptr->object.toString();
    else
        return QString();
}

bool Body::hasJsonObject() const
{
    return d_ptr->object.metaType() == QMetaType::fromType<QJsonObject>();
}

QJsonObject Body::jsonObject() const
{
    return d_ptr->object.toJsonObject();
}

bool Body::hasJsonArray() const
{
    return d_ptr->object.metaType() == QMetaType::fromType<QJsonArray>();
}

QJsonArray Body::jsonArray() const
{
    return d_ptr->object.toJsonArray();
}

bool Body::isDevice() const
{
    return d_ptr->objectType == Type::IODevice;
}

QIODevice* Body::device() const
{
    return d_ptr->object.value<QIODevice *>();
}

bool Body::isMultiPart() const
{
    return d_ptr->objectType == Type::HttpMultiPart;
}

QHttpMultiPart* Body::multiPart() const
{
    return d_ptr->object.value<QHttpMultiPart *>();
}

QVariant Body::object() const
{
    return d_ptr->object;
}

Body::Type Body::objectType() const
{
    return d_ptr->objectType;
}

QString Body::contentType() const
{
    return d_ptr->contentType;
}

HeaderList Body::headers() const
{
    HeaderList headers = d_ptr->headers;
    headers.addParameter("Content-Type", d_ptr->contentType);
    return headers;
}

QMimeDatabase Body::s_mimeDatabase;

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

}
