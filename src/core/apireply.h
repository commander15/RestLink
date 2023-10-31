#ifndef RESTLINK_APIREPLY_H
#define RESTLINK_APIREPLY_H

#include <RestLink/global.h>

#include <QtCore/qobject.h>

class QNetworkRequest;
class QNetworkReply;

namespace RestLink {
class ApiRequest;
class Api;

class ApiReplyPrivate;
class RESTLINK_EXPORT ApiReply : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray downloadedData READ downloadedData NOTIFY finished)
    Q_PROPERTY(int networkError READ networkError NOTIFY networkErrorOccured)
    Q_PROPERTY(QString networkErrorString READ networkErrorString NOTIFY networkErrorOccured)
    Q_PROPERTY(int httpStatusCode READ httpStatusCode NOTIFY finished)
    Q_PROPERTY(QString httpReasonPhrase READ httpReasonPhrase NOTIFY finished)
    Q_PROPERTY(QUrl requestedUrl READ requestedUrl CONSTANT)

public:
    virtual ~ApiReply();

    ApiRequest apiRequest() const;
    Api *api() const;

    QByteArray downloadedData() const;
    Q_SIGNAL void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    Q_SIGNAL void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    Q_SIGNAL void finished();

    int networkError() const;
    QString networkErrorString() const;
    Q_SIGNAL void networkErrorOccured(int error);

    Q_SLOT void abort();

    int httpStatusCode() const;
    QString httpReasonPhrase() const;

    QUrl requestedUrl() const;
    QNetworkRequest networkRequest() const;

    QNetworkReply *networkReply() const;

protected:
    ApiReply(Api *api);
    ApiReply(ApiReplyPrivate *dd, Api *api);

    void setApiRequest(const ApiRequest &request);
    void setNetworkReply(QNetworkReply *reply);

    virtual void processData();

    QScopedPointer<ApiReplyPrivate> d;

private:
    Q_SLOT void downloadPart();
    Q_SLOT void completeDownload();

    friend class Api;
};

}

#endif // RESTLINK_APIREPLY_H
