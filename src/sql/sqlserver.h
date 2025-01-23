#ifndef RESTLINK_SQLSERVER_H
#define RESTLINK_SQLSERVER_H

#include <RestLink/global.h>
#include <RestLink/server.h>

class QJsonParseError;

class QSqlQuery;
class QSqlError;

namespace RestLink {

class ServerResponse;

class SqlServerPrivate;
class RESTLINK_EXPORT SqlServer : public Server
{
    Q_OBJECT

public:
    explicit SqlServer(QObject *parent = nullptr);

    QStringList supportedSchemes() const override;

protected:
    bool init() override;
    void cleanup() override;
    bool maintain() override;

    void handleGet(const Request &request, QSqlQuery *query, ServerResponse *response);
    void handlePost(const Request &request, const QJsonObject &body, QSqlQuery *query, ServerResponse *response);
    void handlePut(const Request &request, const QJsonObject &body, QSqlQuery *query, ServerResponse *response);
    void handlePatch(const Request &request, const QJsonObject &body, QSqlQuery *query, ServerResponse *response);
    void handleDelete(const Request &request, QSqlQuery *query, ServerResponse *response);
    void handleError(const QJsonParseError &error, ServerResponse *response);
    void handleError(QSqlQuery *query, ServerResponse *response);
    void handleError(const QSqlError &error, ServerResponse *response);

    QJsonObject validate(ApiBase::Operation operation, const Request &request, const QJsonObject &body, ServerResponse *response) const;

    void processRequest(ApiBase::Operation operation, const Request &request, const Body &body, Response *r) override;
    Response *createResponse(ApiBase::Operation operation, const Request &request, const Body &body, Api *api) override;
};

} // namespace RestLink

#endif // RESTLINK_SQLSERVER_H
