#ifndef RESTLINK_NETWORKRESPONSE_H
#define RESTLINK_NETWORKRESPONSE_H

#include <RestLink/global.h>
#include <RestLink/response.h>

namespace RestLink {

class NetworkResponsePrivate;
class RESTLINK_EXPORT NetworkResponse : public Response
{
    Q_OBJECT

public:
    explicit NetworkResponse(Api *api);

    Api::Operation operation() const override;

    QNetworkRequest networkRequest() const override;

    bool isFinished() const override;

    int httpStatusCode() const override;
    QString httpReasonPhrase() const override;

    bool hasHeader(const QByteArray &name) const override;
    QByteArray header(const QByteArray &name) const override;
    QByteArrayList headerList() const override;

    QByteArray readBody() override;

    int networkError() const override;
    QString networkErrorString() const override;

    QNetworkReply *networkReply() const override;

public slots:
    void ignoreSslErrors() override;
    void abort() override;

private:
    void setReply(QNetworkReply *reply);

    friend class Response;
    friend class NetworkManager;
};

} // namespace RestLink

#endif // RESTLINK_NETWORKRESPONSE_H
