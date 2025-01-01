#ifndef RESTLINK_BODY_H
#define RESTLINK_BODY_H

#include <RestLink/global.h>
#include <RestLink/header.h>

#include <QtCore/qstring.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qjsonvalue.h>

class QHttpMultiPart;
class QIODevice;
class QFile;

namespace RestLink {

class RESTLINK_EXPORT File
{
public:
    File(const char *fileName);
    File(const QString &fileName);

    QFile *open() const;

private:
    QString m_fileName;
};

class RESTLINK_EXPORT Body
{
public:
    Body();
    Body(QHttpMultiPart *multiPart);
    Body(const File &file);
    Body(QFile *file);
    Body(QIODevice *device, qint64 size = -1, const QByteArray &contentType = QByteArrayLiteral("text/plain"));
    Body(const char *data, int size = -1, const QByteArray &contentType = QByteArrayLiteral("text/plain"));
    Body(const QByteArray &data, const QByteArray &contentType = QByteArrayLiteral("text/plain"));
    Body(const QString &text, const QByteArray &contentType = QByteArrayLiteral("text/plain"));
    Body(const QJsonObject &object);
    Body(const QJsonArray &array);
    Body(const QJsonDocument &doc);
    Body(const QVariant &value, int type = -1, const QByteArray &contentType = QByteArray());
    ~Body();

    bool isMultiPart() const;
    QHttpMultiPart *multiPart() const;

    bool isDevice() const;
    QIODevice *device() const;

    bool isData() const;
    QByteArray data() const;

    HeaderList headers() const;
    void setHeaders(const HeaderList &headers);

    // For later, plan to create a plugin architecture to handle extra content
    static QByteArray generateData(const QVariant &data, int type = -1);
    static QList<Header> generateHeaders(const QVariant &data, int type = -1);

private:
    QHttpMultiPart *m_multiPart;
    QIODevice *m_device;
    QVariant m_data;

    QByteArray m_type;
    qint64 m_length;
    HeaderList m_headers;
};

}

#endif // RESTLINK_BODY_H
