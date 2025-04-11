#ifndef RESTLINK_SQLSERVER_H
#define RESTLINK_SQLSERVER_H

#include <RestLink/server.h>

#include "sqldatabasemanager.h"

namespace RestLink {

class SqlServer : public Server
{
    Q_OBJECT

public:
    explicit SqlServer(QObject *parent = nullptr);
    ~SqlServer();

    QString handlerName() const override;
    QStringList supportedSchemes() const override;

protected:
    bool init() override;
    void cleanup() override;
    bool maintain() override;

    AbstractController *findController(const ServerRequest &request) const override;
    void prepareController(AbstractController *controller, Api::Operation operation, const ServerRequest &request, ServerResponse *response) override;
    void processRequest(ApiBase::Operation operation, const ServerRequest &request, ServerResponse *response) override;

    void processInternalRequest(ApiBase::Operation operation, const ServerRequest &request, ServerResponse *response) override;

private:
    SqlDatabaseManager *m_manager;
};

} // namespace RestLink

#endif // RESTLINK_SQLSERVER_H
