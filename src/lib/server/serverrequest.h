#ifndef RESTLINK_SERVERREQUEST_H
#define RESTLINK_SERVERREQUEST_H

#include <RestLink/global.h>
#include <RestLink/request.h>
#include <RestLink/abstractrequesthandler.h>
#include <RestLink/body.h>

namespace RestLink {

class ServerRequestPrivate;
class RESTLINK_EXPORT ServerRequest : public Request
{
public:
    ServerRequest();
    ServerRequest(AbstractRequestHandler::Method method, const Request &request, const Body &body);
    ServerRequest(const ServerRequest &);
    ServerRequest(ServerRequest &&);
    ~ServerRequest();

    ServerRequest &operator=(const ServerRequest &);
    ServerRequest &operator=(ServerRequest &&);

    AbstractRequestHandler::Method method() const;

    QString resource() const;
    QVariant identifier() const;

    Body body() const;
};

} // namespace RestLink

#endif // RESTLINK_SERVERREQUEST_H
