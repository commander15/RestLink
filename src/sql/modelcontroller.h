#ifndef MODELCONTROLLER_H
#define MODELCONTROLLER_H

#include <RestLink/resourcecontroller.h>

#include <QtCore/qstring.h>

namespace RestLink {
namespace Sql {

class ModelManager;

class ModelController : public RestLink::AbstractResourceController
{
public:
    ModelController();

    QString endpoint() const override;

    void setManager(ModelManager *manager);

    void index(const ServerRequest &request, ServerResponse *response) override;
    void show(const ServerRequest &request, ServerResponse *response) override;
    void update(const ServerRequest &request, ServerResponse *response) override;
    void store(const ServerRequest &request, ServerResponse *response) override;
    void destroy(const ServerRequest &request, ServerResponse *response) override;

    bool canProcessRequest(const ServerRequest &request) const override;
    void processRequest(RequestHandler::Method method, const ServerRequest &request, ServerResponse *response) override;

private:
    QString m_endpoint;
    QString m_table;
    ModelManager *m_manager;
};

} // namespace Sql
} // namespace RestLink

#endif // MODELCONTROLLER_H
