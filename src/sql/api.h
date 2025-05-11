#ifndef API_H
#define API_H

#include "modelcontroller.h"

#include <RestLink/server.h>

namespace RestLink {
namespace Sql {

class Api final : public RestLink::Server
{
    Q_OBJECT

public:
    explicit Api(QObject *parent = nullptr);
    ~Api();

    QString handlerName() const override;
    QStringList supportedSchemes() const override;

protected:
    bool init() override;
    void cleanup() override;
    bool maintain() override;
    void processRequest(const ServerRequest &request, ServerResponse *response) override;

private:
    void processConfigurationRequest(const ServerRequest &request, ServerResponse *response, ModelManager *manager);
    void processTablesRequest(const ServerRequest &request, ServerResponse *response, ModelManager *manager);
    void processQueryRequest(const ServerRequest &request, ServerResponse *response, ModelManager *manager);

    ModelController m_defaultController;
};

} // namespace Sql
} // namespace RestLink

#endif // API_H
