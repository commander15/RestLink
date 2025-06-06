#ifndef ROUTER_H
#define ROUTER_H

#include "modelcontroller.h"

#include <global.h>

#include <RestLink/abstractserverworker.h>

namespace RestLink {
namespace Sql {

class SQL_EXPORT Router final : public RestLink::AbstractServerWorker
{
    Q_OBJECT

public:
    explicit Router(QObject *parent = nullptr);
    ~Router();

protected:
    bool init() override;
    void cleanup() override;
    bool maintain() override;

    void processStandardRequest(const ServerRequest &request, ServerResponse *response) override;

    void *requestDataSource(const ServerRequest &request) override;
    void clearDataSource(const ServerRequest &request, void *source) override;

private:
    void processConfigurationRequest(const ServerRequest &request, ServerResponse *response, Api *api);
    void processDatabaseTablesRequest(const ServerRequest &request, ServerResponse *response, Api *api);
    void processQueryRequest(const ServerRequest &request, ServerResponse *response, Api *api);

    ModelController m_defaultController;
};

} // namespace Sql
} // namespace RestLink

#endif // ROUTER_H
