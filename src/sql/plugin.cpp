#include "plugin.h"

#include "api.h"

namespace RestLink {
namespace Sql {

Plugin::Plugin(QObject *parent)
    : RestLink::Plugin(parent)
{
}

RequestHandler *Plugin::createHandler()
{
    return new Api;
}

} // namespace Sql
} // namespace RestLink
