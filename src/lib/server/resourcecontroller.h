#ifndef RESTLINK_RESOURCEHANDLER_H
#define RESTLINK_RESOURCEHANDLER_H

#include <RestLink/global.h>
#include <RestLink/abstractcontroller.h>

namespace RestLink {

class RESTLINK_EXPORT AbstractResourceController : public AbstractController
{
public:
    virtual ~AbstractResourceController() = default;

    virtual void index(ServerRequest &request, ServerResponse *response) = 0;
    virtual void show(ServerRequest &request, ServerResponse *response) = 0;
    virtual void update(ServerRequest &request, ServerResponse *response) = 0;
    virtual void store(ServerRequest &request, ServerResponse *response) = 0;
    virtual void destroy(ServerRequest &request, ServerResponse *response) = 0;

    bool canProcessRequest(const ServerRequest &request) const override;
    void processRequest(ServerRequest &request, ServerResponse *response) override;
};

} // namespace RestLink

#endif // RESTLINK_RESOURCEHANDLER_H
