#include "sqldatabasemanager.h"

namespace RestLink {

SqlDatabaseManager::SqlDatabaseManager()
{
}

SqlDatabaseManager::~SqlDatabaseManager()
{
    const QList<QUrl> urls = m_databases.keys();
    for (const QUrl &url : urls)
        delete m_databases.take(url);
}

SqlDatabase *SqlDatabaseManager::database(const QUrl &url)
{
    SqlDatabase *db = nullptr;

    if (m_databases.contains(url)) {
        db = m_databases.value(url);
    } else {
        db = new SqlDatabase(url);
        m_databases.insert(url, db);
    }

    if (db && !db->isOpen())
        db->open();

    return db;
}

} // namespace RestLink
