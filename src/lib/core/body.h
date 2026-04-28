#ifndef RESTLINK_BODY_H
#define RESTLINK_BODY_H

#include <RestLink/global.h>
#include <RestLink/header.h>

#include <QtCore/qstring.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qjsonvalue.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qmimedatabase.h>

#define RESTLINK_MIME_PLAIN_TEXT   "text/plain"
#define RESTLINK_MIME_OCTET_STREAM "application/octet-stream"
#define RESTLINK_MIME_JSON         "application/json"

class QHttpMultiPart;
class QIODevice;
class QFile;

namespace RestLink {

class File;

class BodyData;
class RESTLINK_EXPORT Body
{
public:
    enum Type {
        PlainText,
        JsonData,
        RawData,
        IODevice,
        HttpMultiPart,

        Unknown = -1,
    };

    Body();
    Body(const char *data, int size = -1, const QByteArray &contentType = QByteArray());
    Body(const QByteArray &data, const QByteArray &contentType = QByteArray());
    Body(const QString &text, const QByteArray &contentType = QByteArray());
    Body(const QJsonObject &object);
    Body(const QJsonArray &array);
    Body(const QJsonDocument &doc);
    Body(const File &file);
    Body(QFile *file);
    Body(QIODevice *device, const QByteArray &contentType = QByteArray());
    Body(QIODevice *device, qint64 size, const QByteArray &contentType = QByteArray());
    Body(QHttpMultiPart *multiPart);
    Body(const Body &other);
    Body(Body &&other);
    ~Body();

    Body &operator=(const Body &other);
    Body &operator=(Body &&other);

    bool hasPlainText() const;
    QByteArray toByteArray() const;
    QString toString() const;

    bool hasJsonObject() const;
    QJsonObject jsonObject() const;

    bool hasJsonArray() const;
    QJsonArray jsonArray() const;

    bool isDevice() const;
    QIODevice *device() const;

    bool isMultiPart() const;
    QHttpMultiPart *multiPart() const;

    QVariant object() const;
    Type objectType() const;

    QString contentType() const;
    qint64 contentLength() const;

    HeaderList headers() const;

private:
    Body(const QVariant &object, Type type, const QByteArray &contentType, qint64 contentLength);

    QExplicitlySharedDataPointer<BodyData> d_ptr;

    static QMimeDatabase s_mimeDatabase;
};

class RESTLINK_EXPORT File
{
public:
    File(const char *fileName) : m_fileName(fileName) {}
    File(const QString &fileName) : m_fileName(fileName) {}
    File(const File &other) = default;

    QFile *open() const;

private:
    QString m_fileName;
};

}

#endif // RESTLINK_BODY_H
