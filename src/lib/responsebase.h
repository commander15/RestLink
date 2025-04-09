#ifndef RESTLINK_RESPONSEBASE_H
#define RESTLINK_RESPONSEBASE_H

#include <RestLink/global.h>

#include <QtCore/qiodevice.h>

class QJsonParseError;

namespace RestLink {

class RESTLINK_EXPORT ResponseBase : public QIODevice
{
    Q_OBJECT

public:
    explicit ResponseBase(QObject *parent = nullptr);
    virtual ~ResponseBase();

    bool isSequential() const override;
    bool open(OpenMode mode) override;
    void close() override;
    qint64 pos() const override;
    qint64 size() const override;
    bool seek(qint64 pos) override;
    bool atEnd() const override;
    bool reset() override;
    qint64 bytesAvailable() const override;
    qint64 bytesToWrite() const override;
    bool canReadLine() const override;
    bool waitForReadyRead(int msecs) override;
    bool waitForBytesWritten(int msecs) override;

    QJsonObject readJsonObject(QJsonParseError *error = nullptr);
    QJsonArray readJsonArray(QJsonParseError *error = nullptr);
    QJsonValue readJson(QJsonParseError *error = nullptr);
    QString readString();
    virtual QByteArray readBody();

    QIODevice *responseDevice() const;

protected:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 readLineData(char *data, qint64 maxlen) override;
    qint64 skipData(qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 len) override;

    void setResponseDevice(QIODevice *device);

private:
    QIODevice *m_device;
};

} // namespace RestLink

#endif // RESTLINK_RESPONSEBASE_H
