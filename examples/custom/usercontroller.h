#ifndef USERCONTROLLER_H
#define USERCONTROLLER_H

#include <RestLink/resourcecontroller.h>

class UserController : public RestLink::AbstractResourceController
{
public:
    ~UserController()
    { qDebug("Bye bye"); }

    QString endpoint() const override
    { return QStringLiteral("/app/users"); }

    void index(const RestLink::ServerRequest &request, RestLink::ServerResponse *response) override;
    void show(const RestLink::ServerRequest &request, RestLink::ServerResponse *response) override;
    void update(const RestLink::ServerRequest &request, RestLink::ServerResponse *response) override;
    void store(const RestLink::ServerRequest &request, RestLink::ServerResponse *response) override;
    void destroy(const RestLink::ServerRequest &request, RestLink::ServerResponse *response) override;

private:
    class QSqlDatabase database() const;
};

#endif // USERCONTROLLER_H
