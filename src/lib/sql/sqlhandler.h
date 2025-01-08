#ifndef RESTLINK_SQLHANDLER_H
#define RESTLINK_SQLHANDLER_H

#include <RestLink/global.h>
#include <RestLink/threadednetworkhandler.h>

class QSqlQuery;
class QSqlRecord;

namespace RestLink {

class SqlResponse;

class RESTLINK_EXPORT SqlHandler : public ThreadedNetworkHandler
{
    Q_OBJECT

public:
    explicit SqlHandler(QObject *parent = nullptr);

    QStringList schemes() const override;

protected:
    Response *sendRequest(ApiBase::Operation operation, const Request &request, const Body &body, Api *api) override;

    void process(Response *response) override;
    void processQuery(QSqlQuery *query, QJsonObject *object, SqlResponse *response);
    void handleGetOperation(QSqlQuery *query, QJsonObject *object, SqlResponse *response);

    bool init() override;
    void cleanup() override;

    QString generateQuery(Api::Operation operation, const Request &request, const QJsonValue &body, bool *single) const;

private:
    QString dbConnectionForUrl(const QUrl &url);

    QHash<QUrl, QString> m_dbUrl;
};

} // namespace RestLink

#endif // RESTLINK_SQLHANDLER_H
