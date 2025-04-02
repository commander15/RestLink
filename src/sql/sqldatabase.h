#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <RestLink/request.h>

#include <QtSql/qsqldatabase.h>

#include <QtCore/qstring.h>
#include <QtCore/qjsonobject.h>

namespace RestLink {

class SqlDatabase
{
public:
    SqlDatabase(const QString &connection);
    SqlDatabase(const SqlDatabase &other);

    void configure(const Request &request, const QJsonObject &object, Response *response);
    void query(const Request &request, const QString &query, Response *response);

    void index(const Request &request, Response *response);
    void show(const Request &request, Response *response);
    void update(const Request &request, const QJsonObject &object, Response *response);
    void store(const Request &request, const QJsonObject &object, Response *response);
    void destroy(const Request &request, Response *response);

    QString id(const Request &request) const;

    QString table(const Request &request) const;
    QStringList fields(const Request &request) const;
    QStringList relations(const Request &request) const;

    QSqlDriver *sqlDriver() const;
    QSqlDatabase sqlDatabase() const;

private:
    QString m_connection;
    QJsonObject m_configuration;
};

} // namespace RestLink

#endif // SQLDATABASE_H
