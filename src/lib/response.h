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
    Q_PROPERTY(int method READ method CONSTANT FINAL)
    Q_PROPERTY(QString endpoint READ endpoint CONSTANT FINAL)
    Q_PROPERTY(QUrl url READ url CONSTANT FINAL)
    Q_PROPERTY(bool running READ isRunning NOTIFY finished FINAL)
    Q_PROPERTY(bool finished READ isFinished NOTIFY finished FINAL)
    Q_PROPERTY(bool success READ isSuccess NOTIFY finished FINAL)
    Q_PROPERTY(bool hasHttpStatusCode READ hasHttpStatusCode NOTIFY finished FINAL)
    Q_PROPERTY(int httpStatusCode READ httpStatusCode NOTIFY finished FINAL)
    Q_PROPERTY(QString httpReasonPhrase READ httpReasonPhrase NOTIFY finished FINAL)
    Q_PROPERTY(QStringList headerList READ headerList NOTIFY finished FINAL)
    Q_PROPERTY(bool hasNetworkError READ hasNetworkError NOTIFY finished FINAL)
    Q_PROPERTY(int networkError READ networkError NOTIFY networkErrorOccured FINAL)
    Q_PROPERTY(QString networkErrorString READ networkErrorString NOTIFY networkErrorOccured FINAL)
    Q_PROPERTY(QByteArray body READ body NOTIFY finished FINAL)

public:
    virtual ~Response();

    virtual AbstractRequestHandler::Method method() const = 0;
    QString endpoint() const;
    QUrl url() const;
    Request request() const;
    Api *api() const;

    inline bool isRunning() const
    { return !isFinished(); }
    virtual bool isFinished() const = 0;

    inline bool isSuccess() const
    { return !hasNetworkError() && isHttpStatusSuccess(); }

    bool isHttpStatusSuccess() const;
    bool hasHttpStatusCode() const;
    virtual int httpStatusCode() const = 0;
    virtual QString httpReasonPhrase() const;

    virtual bool hasHeader(const QString &name) const;
    Q_INVOKABLE virtual QString header(const QString &name) const = 0;
    virtual QStringList headerList() const = 0;

    inline bool hasNetworkError() const { return networkError() != 0; }
    virtual int networkError() const;
    virtual QString networkErrorString() const;

    virtual QNetworkRequest networkRequest() const = 0;
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

    friend class AbstractRequestHandler;
};

}

#endif // RESTLINK_RESPONSE_H
