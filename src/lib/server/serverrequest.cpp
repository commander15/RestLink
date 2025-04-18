#include "serverrequest.h"

#include <RestLink/body.h>

#include <RestLink/private/request_p.h>

namespace RestLink {

class ServerRequestPrivate : public RequestPrivate
{
public:
    ServerRequestPrivate() = default;
    ServerRequestPrivate(const ServerRequestPrivate &other) = default;
    ServerRequestPrivate(const RequestPrivate *request, const Body &body)
        : RequestPrivate(*request), body(body) {}

    Body body;
};

ServerRequest::ServerRequest()
    : Request(new ServerRequestPrivate())
{
}

ServerRequest::ServerRequest(const Request &request, const Body &body)
    : Request(new ServerRequestPrivate(request.d_ptr.data(), body))
{
}

ServerRequest::ServerRequest(const ServerRequest &rhs)
    : Request(rhs)
{
}

ServerRequest::ServerRequest(ServerRequest &&rhs)
    : Request(std::move(rhs))
{
}

ServerRequest::~ServerRequest()
{
}

ServerRequest &ServerRequest::operator=(const ServerRequest &rhs)
{
    Request::operator=(rhs);
    return *this;
}

ServerRequest &ServerRequest::operator=(ServerRequest &&rhs)
{
    Request::operator=(std::move(rhs));
    return *this;
}

QString ServerRequest::resource() const
{
    QStringList endpoint = d_ptr->endpoint.split('/', Qt::SkipEmptyParts);

    if (endpoint.isEmpty())
        return QString();

    if (endpoint.size() == 1)
        return endpoint.first();

    if (!identifier().isEmpty())
        endpoint.removeLast();
    return endpoint.join('.');
}

QString ServerRequest::identifier() const
{
    const QString last = d_ptr->endpoint.section('/', -1, -1);
    if (last.isEmpty())
        return QString();

    static const QRegularExpression exp("^\\d+$|^[a-fA-F0-9]{8}-([a-fA-F0-9]{4}-){3}[a-fA-F0-9]{12}$");
    const QRegularExpressionMatch match = exp.match(last);
    return (match.hasMatch() ? last : QString());
}

Body ServerRequest::body() const
{
    RESTLINK_D(const ServerRequest);
    return d->body;
}

} // namespace RestLink
