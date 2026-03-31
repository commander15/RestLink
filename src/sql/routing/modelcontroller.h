#ifndef MODELCONTROLLER_H
#define MODELCONTROLLER_H

#include <global.h>
#include <meta/resourceinfo.h>

#include <QtCore/qstring.h>

#include <RestLink/resourcecontroller.h>

class QJsonObject;

namespace RestLink {
namespace Sql {

class Model;
class ResourceInfo;

class Api;

class SQL_EXPORT ModelController : public RestLink::AbstractResourceController
{
public:
    ModelController();

    QString endpoint() const override;

    void init(const ServerRequest &request, Api *api);

    void index(ServerRequest &request, ServerResponse *response) override;
    void show(ServerRequest &request, ServerResponse *response) override;
    void update(ServerRequest &request, ServerResponse *response) override;
    void store(ServerRequest &request, ServerResponse *response) override;
    void destroy(ServerRequest &request, ServerResponse *response) override;

    bool canProcessRequest(const ServerRequest &request) const override;
    void processRequest(ServerRequest &request, ServerResponse *response) override;

    Model requestModel(const ServerRequest &request) const;
    ResourceInfo requestedResource(const ServerRequest &request) const;
    QStringList requestedRelations(const ServerRequest &request, const ResourceInfo &resource) const;

    static int httpStatusCodeFromSqlError(const QJsonObject &error);
    static int httpStatusCodeFromSqlError(int type);

private:
    QString m_endpoint;
    ResourceInfo m_resource;
    Api *m_api;
};

} // namespace Sql
} // namespace RestLink

#endif // MODELCONTROLLER_H
