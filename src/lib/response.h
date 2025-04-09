#ifndef RESTLINK_RESPONSE_H
#define RESTLINK_RESPONSE_H

#include <RestLink/global.h>
#include <RestLink/responsebase.h>
#include <RestLink/api.h>

#include <QtCore/qobject.h>

class QNetworkRequest;
class QNetworkReply;
class QSslError;

class QJsonParseError;

namespace RestLink {

class Request;

class ResponsePrivate;
class NetworkResponsePrivate;
class RESTLINK_EXPORT Response : public ResponseBase
{
    Q_OBJECT
    Q_PROPERTY(QString endpoint READ endpoint CONSTANT)
    Q_PROPERTY(RestLink::Api::Operation operation READ operation CONSTANT)
    Q_PROPERTY(QUrl url READ url CONSTANT)
    Q_PROPERTY(bool running READ isRunning NOTIFY finished)
    Q_PROPERTY(bool finished READ isFinished NOTIFY finished)
    Q_PROPERTY(bool success READ isSuccess NOTIFY finished)
    Q_PROPERTY(bool hasHttpStatusCode READ hasHttpStatusCode NOTIFY finished)
    Q_PROPERTY(int httpStatusCode READ httpStatusCode NOTIFY finished)
    Q_PROPERTY(QString httpReasonPhrase READ httpReasonPhrase NOTIFY finished)
    Q_PROPERTY(QByteArrayList headerList READ headerList NOTIFY finished)
    Q_PROPERTY(bool hasNetworkError READ hasNetworkError NOTIFY finished)
    Q_PROPERTY(int networkError READ networkError NOTIFY networkErrorOccured)
    Q_PROPERTY(QString networkErrorString READ networkErrorString NOTIFY networkErrorOccured)
    Q_PROPERTY(QByteArray body READ body NOTIFY finished)

public:
    virtual ~Response();

    QString endpoint() const;
    Request request() const;

    virtual Api::Operation operation() const = 0;
    Api *api() const;

    QUrl url() const;
    virtual QNetworkRequest networkRequest() const = 0;

    inline bool isRunning() const
    { return !isFinished(); }
    virtual bool isFinished() const = 0;

    inline bool isSuccess() const
    { return !hasNetworkError() && isHttpStatusSuccess(); }

    bool isHttpStatusSuccess() const;
    bool hasHttpStatusCode() const;
    virtual int httpStatusCode() const = 0;
    virtual QString httpReasonPhrase() const;

    virtual bool hasHeader(const QByteArray &name) const;
    Q_INVOKABLE virtual QByteArray header(const QByteArray &name) const = 0;
    virtual QByteArrayList headerList() const = 0;

    inline bool hasNetworkError() const { return networkError() != 0; }
    virtual int networkError() const;
    virtual QString networkErrorString() const;

    virtual QNetworkReply *networkReply() const = 0;

protected:
    Response(ResponsePrivate *d, QObject *parent);

    void setRequest(const Request &request);

    QByteArray body();

    QScopedPointer<ResponsePrivate> d_ptr;

public slots:
    virtual void ignoreSslErrors();
    virtual void abort() = 0;

signals:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void networkErrorOccured(int error);
    void finished();

    void sslErrorsOccured(const QList<QSslError> &errors);

    friend class RequestHandler;
};

}

#endif // RESTLINK_RESPONSE_H
