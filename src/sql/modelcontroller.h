#ifndef MODELCONTROLLER_H
#define MODELCONTROLLER_H

#include <RestLink/resourcecontroller.h>

#include <QtCore/qstring.h>

namespace RestLink {
namespace Sql {

class Model;

class Api;

class ModelController : public RestLink::AbstractResourceController
{
public:
    ModelController();

    QString endpoint() const override;

    void setApi(Api *api);

    void index(const ServerRequest &request, ServerResponse *response) override;
    void show(const ServerRequest &request, ServerResponse *response) override;
    void update(const ServerRequest &request, ServerResponse *response) override;
    void store(const ServerRequest &request, ServerResponse *response) override;
    void destroy(const ServerRequest &request, ServerResponse *response) override;

    bool canProcessRequest(const ServerRequest &request) const override;
    void processRequest(const ServerRequest &request, ServerResponse *response) override;

    Model currentModel(const ServerRequest &request) const;

    static int httpStatusCodeFromSqlError(int type);

private:
    QString m_endpoint;
    QString m_resource;
    Api *m_api;
};

} // namespace Sql
} // namespace RestLink

#endif // MODELCONTROLLER_H
