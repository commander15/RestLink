#ifndef RESTLINK_REQUESTINTERCEPTOR_H
#define RESTLINK_REQUESTINTERCEPTOR_H

#include <RestLink/global.h>
#include <RestLink/api.h>

namespace RestLink {

class RESTLINK_EXPORT RequestInterceptor
{
public:
    virtual ~RequestInterceptor() = default;

    virtual Request intercept(const Request &request, const Body &body, RequestHandler::Method method) = 0;
};

}

#endif // RESTLINK_REQUESTINTERCEPTOR_H
