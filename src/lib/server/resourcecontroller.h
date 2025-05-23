#ifndef RESTLINK_RESOURCEHANDLER_H
#define RESTLINK_RESOURCEHANDLER_H

#include <RestLink/global.h>
#include <RestLink/abstractcontroller.h>

namespace RestLink {

class RESTLINK_EXPORT AbstractResourceController : public AbstractController
{
public:
    virtual ~AbstractResourceController() = default;

    virtual void index(const ServerRequest &request, ServerResponse *response) = 0;
    virtual void show(const ServerRequest &request, ServerResponse *response) = 0;
    virtual void update(const ServerRequest &request, ServerResponse *response) = 0;
    virtual void store(const ServerRequest &request, ServerResponse *response) = 0;
    virtual void destroy(const ServerRequest &request, ServerResponse *response) = 0;

    bool canProcessRequest(const ServerRequest &request) const override;
    void processRequest(const ServerRequest &request, ServerResponse *response) override;
};

} // namespace RestLink

#endif // RESTLINK_RESOURCEHANDLER_H
