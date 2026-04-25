#include "worker.h"

#include <QSqlQuery>

Worker::Worker(QObject *parent)
    : RestLink::AbstractServerWorker{RestLink::AbstractServerWorker::Synchronous, parent}
    , m_users(new UserController)
    , m_conn{"CON"}
{
    registerController(m_users);
}

bool Worker::init()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_conn);
    db.setDatabaseName("custom.db");
    if (!db.open())
        return false;

    auto run = [&db](const QString &stmt) -> bool {
        QSqlQuery query(db);
        return query.exec(stmt);
    };

    run(R"(
        CREATE TABLE IF NOT EXISTS Users(
            id         INTEGER PRIMARY KEY AUTOINCREMENT,
            first_name VARCHAR(30),
            last_name  VARCHAR(60) NOT NULL,
            gender     CHAR(1) NOT NULL CHECK(gender = 'M' OR gender = 'F')
        )
    )");

    return true;
}

void Worker::cleanup()
{
    QSqlDatabase::removeDatabase(m_conn);
}

bool Worker::maintain()
{
    // No maintenance routines
    return true;
}

void *Worker::createDataSource(const RestLink::ServerRequest &request)
{
    Q_UNUSED(request)
    return &m_conn;
}

void Worker::clearDataSource(const RestLink::ServerRequest &request, void *source)
{
    Q_UNUSED(request)
    Q_UNUSED(source)
    // No data source
}
