#ifndef WORKER_H
#define WORKER_H

#include <RestLink/abstractserverworker.h>

#include "usercontroller.h"

#include <QSqlDatabase>

class Worker : public RestLink::AbstractServerWorker
{
    Q_OBJECT

public:
    explicit Worker(QObject *parent = nullptr);

protected:
    bool init() override;
    void cleanup() override;
    bool maintain() override;
    void *createDataSource(const RestLink::ServerRequest &request) override;
    void clearDataSource(const RestLink::ServerRequest &request, void *source) override;

private:
    UserController *m_users;
    QString m_conn;
};

#endif // WORKER_H
