#ifndef RESTLINK_SERVERRESPONSE_H
#define RESTLINK_SERVERRESPONSE_H

#include <RestLink/global.h>
#include <RestLink/response.h>

namespace RestLink {

class Server;

class RESTLINK_EXPORT ServerResponse : public RestLink::Response
{
    Q_OBJECT

public:
    explicit ServerResponse(Server *server);

    AbstractRequestHandler::Method method() const override;
    void setMethod(AbstractRequestHandler::Method method);

    bool isFinished() const override;

    int httpStatusCode() const override;
    void setHttpStatusCode(int code);

    bool hasHeader(const QString &name) const override;
    QString header(const QString &name) const override;
    QStringList headerList() const override;
    void setHeaders(const QList<Header> &headers);

    QJsonObject readJsonObject(QJsonParseError *error) override;
    QJsonArray readJsonArray(QJsonParseError *error) override;
    QJsonValue readJson(QJsonParseError *error) override;
    QString readString() override;
    QByteArray readBody() override;
    void setBody(const Body &body);

    QNetworkRequest networkRequest() const override;
    void setNetworkRequest(const QNetworkRequest &request);

    QNetworkReply *networkReply() const override;

    Server *server() const;

public slots:
    void updateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void updateUploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void complete();

    void ignoreSslErrors() override;
    void abort() override;
};

} // namespace RestLink

#endif // RESTLINK_SERVERRESPONSE_H
