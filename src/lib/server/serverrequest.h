#ifndef RESTLINK_SERVERREQUEST_H
#define RESTLINK_SERVERREQUEST_H

#include <RestLink/global.h>
#include <RestLink/request.h>
#include <RestLink/body.h>

namespace RestLink {

class ServerRequestPrivate;
class RESTLINK_EXPORT ServerRequest : public Request
{
public:
    ServerRequest();
    ServerRequest(const Request &request, const Body &body);
    ServerRequest(const ServerRequest &);
    ServerRequest(ServerRequest &&);
    ~ServerRequest();

    ServerRequest &operator=(const ServerRequest &);
    ServerRequest &operator=(ServerRequest &&);

    QString resource() const;
    QString identifier() const;

    Body body() const;
};

} // namespace RestLink

#endif // RESTLINK_SERVERREQUEST_H
