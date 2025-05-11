#ifndef RESTLINK_BODY_H
#define RESTLINK_BODY_H

#include <RestLink/global.h>
#include <RestLink/header.h>

#include <QtCore/qstring.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qjsonvalue.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qmimedatabase.h>

class QHttpMultiPart;
class QIODevice;
class QFile;

namespace RestLink {

class File;

class BodyData;
class RESTLINK_EXPORT Body
{
public:
    enum class Type {
        PlainText,
        JsonData,
        IODevice,
        HttpMultiPart,

        Unknown = -1,
    };


    Body();
    Body(const char *data, int size = -1, const QByteArray &contentType = QByteArrayLiteral("text/plain"));
    Body(const QByteArray &data);
    Body(const QByteArray &data, const QByteArray &contentType);
    Body(const QString &text);
    Body(const QString &text, const QByteArray &contentType);
    Body(const QJsonValue &value);
    Body(const QJsonObject &object);
    Body(const QJsonArray &array);
    Body(const QJsonDocument &doc);
    Body(const QVariant &value, const QByteArray &contentType = QByteArray());
    Body(const File &file);
    Body(QFile *file);
    Body(QIODevice *device, qint64 size = -1, const QByteArray &contentType = QByteArrayLiteral("text/plain"));
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
    //qint64 contentLength() const;

    HeaderList headers() const;

private:
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
