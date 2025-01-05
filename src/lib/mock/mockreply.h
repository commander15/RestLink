#ifndef RESTLINK_MOCKREPLY_H
#define RESTLINK_MOCKREPLY_H

#include <RestLink/global.h>
#include <RestLink/mockmanager.h>

#include <QtNetwork/qnetworkreply.h>

class QBuffer;

namespace RestLink {

class MockReplyPrivate;
class RESTLINK_EXPORT MockReply : public QNetworkReply
{
    Q_OBJECT

public:
    MockReply(const QNetworkRequest &request, QObject *parent = nullptr);

    qint64 pos() const override;
    qint64 size() const override;
    bool seek(qint64 pos) override;
    bool atEnd() const override;
    bool reset() override;

    qint64 bytesAvailable() const override;
    qint64 bytesToWrite() const override;

    static MockReply *create(const QNetworkRequest &request, const MockData &data, QObject *parent = nullptr);

public slots:
    void abort() override;

protected:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

private:
    void setMockData(const MockData &data);

    QIODevice *m_buffer;

    friend class MockManager;
};

} // namespace RestLink

#endif // RESTLINK_MOCKREPLY_H
