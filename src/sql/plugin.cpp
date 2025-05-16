#include <RestLink/plugin.h>

#include <routing/router.h>

namespace RestLink {
namespace Sql {

class Plugin : public RestLink::Plugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID RESTLINK_PLUGIN_IID FILE "metadata.json")

public:
    explicit Plugin(QObject *parent = nullptr)
        : RestLink::Plugin(parent) {}

    AbstractRequestHandler *createHandler() override
    { return new Router; }
};

} // namespace Sql
} // namespace RestLink

#include "plugin.moc"
