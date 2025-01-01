#ifndef RESPONSE_H
#define RESPONSE_H

#include <RestLink/global.h>
#include <RestLink/api.h>

#include <QtCore/qobject.h>

class QNetworkRequest;
class QNetworkReply;
class QSslError;

class QJsonParseError;

namespace RestLink {

class Request;

class ResponsePrivate;
class RESTLINK_EXPORT Response : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString endpoint READ endpoint CONSTANT)
    Q_PROPERTY(RestLink::Api::Operation operation READ operation CONSTANT)
    Q_PROPERTY(QUrl url READ url CONSTANT)
    Q_PROPERTY(bool running READ isRunning NOTIFY finished)
    Q_PROPERTY(bool finished READ isFinished NOTIFY finished)
    Q_PROPERTY(bool success READ isSuccess NOTIFY finished)
    Q_PROPERTY(int httpStatusCode READ httpStatusCode NOTIFY finished)
    Q_PROPERTY(QString httpReasonPhrase READ httpReasonPhrase NOTIFY finished)
    Q_PROPERTY(QByteArrayList headerList READ headerList NOTIFY finished)
    Q_PROPERTY(int networkError READ networkError NOTIFY networkErrorOccured)
    Q_PROPERTY(QString networkErrorString READ networkErrorString NOTIFY networkErrorOccured)
    Q_PROPERTY(QByteArray body READ body NOTIFY finished)

public:
    ~Response();

    QString endpoint() const;
    Request request() const;

    Api::Operation operation() const;
    Api *api() const;

    QUrl url() const;
    QNetworkRequest networkRequest() const;

    bool isRunning() const;
    bool isFinished() const;

    bool isSuccess() const;

    bool isHttpStatusSuccess() const;
    bool hasHttpStatusCode() const;
    int httpStatusCode() const;
    QString httpReasonPhrase() const;

    bool hasHeader(const QByteArray &name) const;
    Q_INVOKABLE QByteArray header(const QByteArray &header) const;
    QByteArrayList headerList() const;

    QJsonObject readJsonObject(QJsonParseError *error = nullptr);
    QJsonArray readJsonArray(QJsonParseError *error = nullptr);
    QJsonValue readJson(QJsonParseError *error = nullptr);
    QString readString();
    Q_INVOKABLE QByteArray readBody();

    bool hasNetworkError() const;
    int networkError() const;
    QString networkErrorString() const;

    QNetworkReply *networkReply() const;

protected:
    Response(const Request &request, QNetworkReply *reply, Api *api);

public slots:
    void ignoreSslErrors();
    void abort();

signals:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void networkErrorOccured(int error);
    void readyRead();
    void finished();

    void sslErrorsOccured(const QList<QSslError> &errors);

private:
    QByteArray body();

    QScopedPointer<ResponsePrivate> d_ptr;

    friend class Api;
};

}

#endif // RESPONSE_H
