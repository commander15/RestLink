#include "body.h"
#include "body_p.h"

#include <RestLink/debug.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qfile.h>
#include <QtCore/qiodevice.h>

#include <QtNetwork/qhttpmultipart.h>

namespace RestLink {

/*!
 * \class RestLink::Body
 * \brief Represents the body of an HTTP request.
 *
 * The Body class provides an abstraction over various types of request payloads,
 * including plain text, JSON data, binary data, and streaming via QIODevice or multipart form data.
 * It allows convenient construction and inspection of content and its metadata such as content type and length.
 */

/*!
 * \enum Body::Type
 * \brief Describes the type of content contained in the request body.
 *
 * This enum helps identify how the request body should be processed or interpreted.
 *
 * \var Body::Type Body::PlainText
 * Indicates that the body contains plain text data.
 *
 * \var Body::Type Body::JsonData
 * Indicates that the body contains JSON-encoded data.
 *
 * \var Body::Type Body::RawData
 * Indicates that the body contains raw binary data.
 *
 * \var Body::Type Body::IODevice
 * Indicates that the body is streamed from a QIODevice.
 *
 * \var Body::Type Body::HttpMultiPart
 * Indicates that the body is a multipart/form-data payload, typically used for file uploads.
 *
 * \var Body::Type Body::Unknown
 * Indicates an unknown or unsupported body type.
 */

/*!
 * \class RestLink::File
 * \brief Helper class to manage a file intended for request body usage.
 *
 * The File class encapsulates a file path and provides an interface
 * to open the file for reading, typically used in constructing a Body.
 */

/*!
 * \brief Constructs an empty Body.
 */
Body::Body()
    : d_ptr(new BodyData())
{
}

/*!
 * \brief Constructs a Body from a C-style byte array.
 * \param data Pointer to the data.
 * \param size Number of bytes (or -1 for null-terminated).
 * \param contentType MIME type of the content.
 */
Body::Body(const char *data, int size, const QByteArray &contentType)
    : Body(QByteArray(data, size), contentType)
{
}

/*!
 * \brief Constructs a Body from a QByteArray.
 * \param data The content as a QByteArray.
 * \param contentType MIME type of the content.
 */
Body::Body(const QByteArray &data, const QByteArray &contentType)
    : Body(QVariant(data), Type::RawData, contentType.isEmpty() ? RESTLINK_MIME_OCTET_STREAM : contentType, data.length())
{
}
/*!
 * \brief Constructs a Body from a QString.
 * \param text The string content.
 * \param contentType MIME type of the content.
 */
Body::Body(const QString &text, const QByteArray &contentType)
    : Body(QVariant(text), Type::PlainText, contentType.isEmpty() ? RESTLINK_MIME_PLAIN_TEXT : contentType, text.length())
{
}

/*!
 * \brief Constructs a Body from a QJsonObject.
 */
Body::Body(const QJsonObject &object)
    : Body(QJsonDocument(object))
{
}

/*!
 * \brief Constructs a Body from a QJsonArray.
 */
Body::Body(const QJsonArray &array)
    : Body(QJsonDocument(array))
{
}

/*!
 * \brief Constructs a Body from a QJsonDocument.
 */
Body::Body(const QJsonDocument &doc)
    : Body(doc.isObject() ? QVariant::fromValue(doc.object()) : QVariant::fromValue(doc.array()), Type::JsonData, RESTLINK_MIME_JSON, doc.toJson(QJsonDocument::Compact).size())
{
}

/*!
 * \brief Constructs a Body from a RestLink::File.
 * \see File
 */
Body::Body(const File &file)
    : Body(file.open())
{
}

/*!
 * \brief Constructs a Body from a QFile pointer.
 */
Body::Body(QFile *file)
    : Body(file, file->size(), s_mimeDatabase.mimeTypeForFile(file->fileName()).name().toLatin1())
{
}

/*!
 * \brief Constructs a Body from a QIODevice.
 * \param device The device supplying the data.
 * \param contentType MIME type of the content.
 */
Body::Body(QIODevice *device, const QByteArray &contentType)
    : Body(device, device->size(), contentType)
{
}

/*!
 * \brief Constructs a Body from a QIODevice with a known content length.
 * \param device The device supplying the data.
 * \param size Length of the content in bytes.
 * \param contentType MIME type of the content.
 */
Body::Body(QIODevice *device, qint64 size, const QByteArray &contentType)
    : d_ptr(new BodyData())
{
    d_ptr->object = QVariant::fromValue(device);
    d_ptr->objectType = Type::IODevice;

    if (contentType.isEmpty()) {
        if (device->isSequential() && device->isReadable())
            d_ptr->contentType = s_mimeDatabase.mimeTypeForData(device).name().toLatin1();
        else
            d_ptr->contentType = RESTLINK_MIME_OCTET_STREAM;
    } else {
        d_ptr->contentType = contentType;
    }

    d_ptr->contentLength = size > 0 ? size : device->size();
}

/*!
 * \brief Constructs a Body from a QHttpMultiPart.
 */
Body::Body(QHttpMultiPart *multiPart)
    : d_ptr(new BodyData())
{
    d_ptr->object = QVariant::fromValue(multiPart);
    d_ptr->objectType = Type::HttpMultiPart;
}

Body::Body(const QVariant &object, Type type, const QByteArray &contentType, qint64 contentLength)
    : d_ptr(new BodyData())
{
    d_ptr->object = object;
    d_ptr->objectType = type;
    d_ptr->contentLength = contentLength;
    d_ptr->contentType = contentType.isEmpty() ? RESTLINK_MIME_OCTET_STREAM : contentType;
}

/*!
 * \brief Copy constructor.
 */
Body::Body(const Body &other) = default;

/*!
 * \brief Move constructor.
 */
Body::Body(Body &&other) = default;

/*!
 * \brief Destructor.
 */
Body::~Body() = default;

/*!
 * \brief Copy assignment operator.
 */
Body &Body::operator=(const Body &other)
{
    if (this != &other) {
        d_ptr = other.d_ptr;
    }

    return *this;
}

/*!
 * \brief Move assignment operator.
 */
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
    switch (d_ptr->objectType) {
    case Type::RawData:
    case Type::PlainText:
        return d_ptr->object.toByteArray();

    case Type::JsonData:
    {
        const QJsonValue value = d_ptr->object.toJsonValue();
        if (value.isObject())
            return QJsonDocument(value.toObject()).toJson();
        else
            return QJsonDocument(value.toArray()).toJson();
    }

    case Type::IODevice:
        return device()->readAll();

    default:
        return QByteArray();
    }
}

QString Body::toString() const
{
    return QString::fromUtf8(toByteArray());
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

/*!
 * \brief Returns the content type as a MIME string.
 */
QString Body::contentType() const
{
    return d_ptr->contentType;
}

/*!
 * \brief Returns the length of the content, if known.
 */
qint64 Body::contentLength() const
{
    return d_ptr->contentLength;
}

/*!
 * \brief Returns the list of headers associated with the Body.
 *
 * headers like Content-Type and Content-Length are added automatically.
 */
HeaderList Body::headers() const
{
    HeaderList headers = d_ptr->headers;

    if (!d_ptr->contentType.isEmpty())
        headers.addParameter("Content-Type", d_ptr->contentType);

    if (d_ptr->contentLength > 0)
        headers.addParameter("Content-Length", d_ptr->contentLength);

    return headers;
}

QMimeDatabase Body::s_mimeDatabase;

/*!
 * \fn File::File(const char *fileName)
 * \brief Constructs a File from a file path.
 * \param fileName File path.
 */

/*!
 * \fn File::File(const QString &fileName)
 * \brief Constructs a File from a file path.
 * \param fileName File path.
 */

/*!
 * \brief Opens the file and returns a QFile pointer.
 * \warning The file is parented to app instance.
 */
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
