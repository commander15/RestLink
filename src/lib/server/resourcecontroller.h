#ifndef RESTLINK_RESOURCEHANDLER_H
#define RESTLINK_RESOURCEHANDLER_H

#include <RestLink/global.h>
#include <RestLink/serverresponse.h>

namespace RestLink {

class ServerRequest;
class Server;

class RESTLINK_EXPORT AbstractController
{
public:
    explicit AbstractController();
    virtual ~AbstractController();

    virtual QString endpoint() const = 0;

    void *dataSource() const;
    void setDataSource(void *source);

    virtual bool canProcessRequest(const ServerRequest &request) const;
    virtual void processRequest(ApiBase::Operation operation, const ServerRequest &request, ServerResponse *response) = 0;

private:
    void *m_dataSource;
};

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
    void processRequest(ApiBase::Operation operation, const ServerRequest &request, ServerResponse *response) override;
};

} // namespace RestLink

#endif // RESTLINK_RESOURCEHANDLER_H
