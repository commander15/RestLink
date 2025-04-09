#ifndef SQLDATABASEMANAGER_H
#define SQLDATABASEMANAGER_H

#include "sqldatabase.h"

namespace RestLink {

class SqlDatabaseManager
{
public:
    SqlDatabaseManager();
    ~SqlDatabaseManager();

    SqlDatabase *database(const QUrl &url);

private:
    QHash<QUrl, SqlDatabase *> m_databases;
};

} // namespace RestLink

#endif // SQLDATABASEMANAGER_H
