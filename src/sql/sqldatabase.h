#ifndef RESTLINK_SQLDATABASE_H
#define RESTLINK_SQLDATABASE_H

#include <RestLink/resourcecontroller.h>

#include <QtSql/qsqldatabase.h>

#include <QtCore/qurl.h>
#include <QtCore/qjsonobject.h>

namespace RestLink {

class ServerRequest;
class ServerResponse;

class SqlDatabase : public QSqlDatabase, public AbstractResourceController
{
public:
    SqlDatabase(const QUrl &url);

    QString endpoint() const override;
    QUrl url() const;

    void updateConfiguration(const QJsonObject &config);

    bool canProcessRequest(const ServerRequest &request) const override;
    void processRequest(ApiBase::Operation operation, const ServerRequest &request, ServerResponse *response) override;

    void query(const ServerRequest &request, ServerResponse *response);
    void tables(const ServerRequest &request, ServerResponse *response);

    void index(const ServerRequest &request, ServerResponse *response) override;
    void show(const ServerRequest &request, ServerResponse *response) override;
    void update(const ServerRequest &request, ServerResponse *response) override;
    void store(const ServerRequest &request, ServerResponse *response) override;
    void destroy(const ServerRequest &request, ServerResponse *response) override;

    QString tableName(const ServerRequest &request) const;
    QSqlRecord tableRecord(const ServerRequest &request, bool filled = true) const;

    QJsonObject configurationFor(const QString &tableName) const;

    QSqlQuery exec(const QString &query, ServerResponse *response = nullptr);

private:
    bool canProcess(const ServerRequest &request, ServerResponse *response) const;

    QString selectStatement(const ServerRequest &request) const;
    QString countStatement(const ServerRequest &request) const;
    QString deleteStatement(const ServerRequest &request) const;

    QString whereClause(const ServerRequest &request, bool idOnly = false) const;
    QString orderByClause(const ServerRequest &request) const;
    QString limitOffsetClause(const ServerRequest &request, int *page, int *items) const;

    void handleError(const QSqlError &error, ServerResponse *response);

    const QUrl m_url;
    QJsonObject m_configuration;
};

} // namespace RestLink

#endif // RESTLINK_SQLDATABASE_H
