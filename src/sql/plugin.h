#ifndef PLUGIN_H
#define PLUGIN_H

#include <RestLink/plugin.h>

namespace RestLink {
namespace Sql {

class Plugin : public RestLink::Plugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID RESTLINK_PLUGIN_IID FILE "metadata.json")

public:
    explicit Plugin(QObject *parent = nullptr);

    AbstractRequestHandler *createHandler() override;
};

} // namespace Sql
} // namespace RestLink

#endif // PLUGIN_H
