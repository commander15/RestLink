#ifndef USERCONTROLLER_H
#define USERCONTROLLER_H

#include <RestLink/resourcecontroller.h>

class UserController : public RestLink::AbstractResourceController
{
public:
    ~UserController()
    { qDebug("Bye bye"); }

    QString endpoint() const override;

    void index(RestLink::ServerRequest &request, RestLink::ServerResponse *response) override;
    void show(RestLink::ServerRequest &request, RestLink::ServerResponse *response) override;
    void update(RestLink::ServerRequest &request, RestLink::ServerResponse *response) override;
    void store(RestLink::ServerRequest &request, RestLink::ServerResponse *response) override;
    void destroy(RestLink::ServerRequest &request, RestLink::ServerResponse *response) override;

private:
    class QSqlDatabase database() const;
};

#endif // USERCONTROLLER_H
