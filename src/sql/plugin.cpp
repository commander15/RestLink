#include "plugin.h"

#include "router.h"

namespace RestLink {
namespace Sql {

Plugin::Plugin(QObject *parent)
    : RestLink::Plugin(parent)
{
}

AbstractRequestHandler *Plugin::createHandler()
{
    return new Router;
}

} // namespace Sql
} // namespace RestLink
