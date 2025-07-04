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
    explicit NetworkResponse(QObject *parent);

    AbstractRequestHandler::Method method() const override;

    QNetworkRequest networkRequest() const override;

    bool isFinished() const override;

    int httpStatusCode() const override;
    QString httpReasonPhrase() const override;

    bool hasHeader(const QString &name) const override;
    QString header(const QString &name) const override;
    QStringList headerList() const override;

    QByteArray readBody() override;

    int networkError() const override;
    QString networkErrorString() const override;

    QNetworkReply *networkReply() const override;

public slots:
    void ignoreSslErrors() override;
    void abort() override;

private:
    void setReply(QNetworkReply *reply);

    friend class NetworkManager;
};

} // namespace RestLink

#endif // RESTLINK_NETWORKRESPONSE_H
