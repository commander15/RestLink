#ifndef RESTLINK_SQLSERVER_P_H
#define RESTLINK_SQLSERVER_P_H

#include "sqlserver.h"

#include <RestLink/private/server_p.h>

class QSqlDatabase;

namespace RestLink {

class SqlServerPrivate : public ServerPrivate
{
public:
    struct DatabaseInfos {
        QString connection;
    };

    SqlServerPrivate(SqlServer *q);

    static int extractDataFromPath(const QString &path, QString *table, QString *primary);
    static void extractDataFromQueries(const QList<QueryParameter> &queries, QStringList *filters, QString *sortField, Qt::SortOrder *sortOrder, int *page, int *limit);

    QSqlDatabase databaseFromUrl(const QUrl &url);

    QHash<QUrl, DatabaseInfos> databaseInfos;
};

} // namespace RestLink

#endif // RESTLINK_SQLSERVER_P_H
