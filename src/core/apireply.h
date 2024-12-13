#ifndef RESTLINK_APIREPLY_H
#define RESTLINK_APIREPLY_H

#include <RestLink/global.h>
#include <RestLink/api.h>

#include <QtCore/qobject.h>

class QNetworkRequest;
class QNetworkReply;

class QJsonParseError;

namespace RestLink {
class ApiRequest;

class ApiReplyPrivate;
class RESTLINK_EXPORT ApiReply : public QObject
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

public:
    virtual ~ApiReply();

    QString endpoint() const;
    ApiRequest apiRequest() const;

    Api::Operation operation() const;
    Api *api() const;

    QUrl url() const;
    QNetworkRequest networkRequest() const;

    bool isRunning() const;
    bool isFinished() const;
    Q_SLOT void abort();
    Q_SIGNAL void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    Q_SIGNAL void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    Q_SIGNAL void finished();

    bool isSuccess() const;

    bool isHttpStatusSuccess() const;
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
    Q_SIGNAL void readyRead();

    Q_SLOT void ignoreSslErros(const QList<QSslError> &errors);
    Q_SIGNAL void sslErrorsOccured(const QList<QSslError> &errors);

    bool hasNetworkError() const;
    int networkError() const;
    QString networkErrorString() const;
    Q_SIGNAL void networkErrorOccured(int error);

    QNetworkReply *networkReply() const;

protected:
    ApiReply(Api *api);
    ApiReply(ApiReplyPrivate *dd, Api *api);

    void setApiRequest(const ApiRequest &request);
    void setNetworkReply(QNetworkReply *reply);

    QScopedPointer<ApiReplyPrivate> d;

    friend class Api;
};

}

#endif // RESTLINK_APIREPLY_H
