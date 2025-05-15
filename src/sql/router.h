#ifndef ROUTER_H
#define ROUTER_H

#include "modelcontroller.h"

#include <RestLink/server.h>

namespace RestLink {
namespace Sql {

class Router final : public RestLink::Server
{
    Q_OBJECT

public:
    explicit Router(QObject *parent = nullptr);
    ~Router();

    QString handlerName() const override;
    QStringList supportedSchemes() const override;

protected:
    bool init() override;
    void cleanup() override;
    bool maintain() override;
    void processRequest(const ServerRequest &request, ServerResponse *response) override;

private:
    void processConfigurationRequest(const ServerRequest &request, ServerResponse *response, Api *manager);
    void processQueryRequest(const ServerRequest &request, ServerResponse *response, Api *manager);

    ModelController m_defaultController;
};

} // namespace Sql
} // namespace RestLink

#endif // ROUTER_H
