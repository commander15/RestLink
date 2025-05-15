#ifndef RESTLINK_ABSTRACTCONTROLLER_H
#define RESTLINK_ABSTRACTCONTROLLER_H

#include <RestLink/global.h>

namespace RestLink {

class ServerRequest;
class ServerResponse;

class RESTLINK_EXPORT AbstractController
{
public:
    explicit AbstractController();
    virtual ~AbstractController();

    virtual QString endpoint() const = 0;

    virtual bool canProcessRequest(const ServerRequest &request) const;
    virtual void processRequest(const ServerRequest &request, ServerResponse *response) = 0;

    void *dataSource() const;
    void setDataSource(void *source);

private:
    void *m_dataSource;
};

} // namespace RestLink

#endif // RESTLINK_ABSTRACTCONTROLLER_H
