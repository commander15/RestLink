#ifndef RESTLINK_INTERNALRESPONSE_H
#define RESTLINK_INTERNALRESPONSE_H

#include <RestLink/global.h>
#include <RestLink/response.h>

namespace RestLink {

class InternalResponsePrivate;
class RESTLINK_EXPORT InternalResponse : public Response
{
    Q_OBJECT

public:
    virtual ~InternalResponse();

    Api::Operation operation() const override;
    void setOperation(Api::Operation operation);

    bool isFinished() const override;

    int httpStatusCode() const override;
    void setHttpStatusCode(int code);

    bool hasHeader(const QByteArray &name) const override;
    QByteArray header(const QByteArray &name) const override;
    QByteArrayList headerList() const override;
    void setHeaders(const QList<Header> &headers);

    QJsonValue readJson(QJsonParseError *error) override;
    QByteArray readBody() override;
    void setData(const QJsonValue &data);
    void setData(const QByteArray &data);

    QNetworkRequest networkRequest() const override;
    void setNetworkRequest(const QNetworkRequest &request);

    QNetworkReply *networkReply() const override;

public slots:
    void updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void updateUploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void complete();

    void ignoreSslErrors() override;
    void abort() override;

protected:
    InternalResponse(InternalResponsePrivate *d, Api *api);
};

} // namespace RestLink

#endif // RESTLINK_INTERNALRESPONSE_H
