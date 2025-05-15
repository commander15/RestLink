#ifndef RESTLINK_PLUGINMANAGER_H
#define RESTLINK_PLUGINMANAGER_H

#include <RestLink/global.h>
#include <RestLink/plugin.h>

namespace RestLink {

class PluginManagerPrivate;
class RESTLINK_EXPORT PluginManager
{
public:
    PluginManager();
    ~PluginManager();

    static QList<AbstractRequestHandler *> handlers();

    static bool isDiscoveryEnabled();
    static void enableDiscovery();
    static void setDiscoveryEnabled(bool enable = true);

    static void registerPlugin(const QString &name);

    static PluginManager *global();

private:
    AbstractRequestHandler *createHandler(Plugin *plugin);
    Plugin *loadPlugin(const QString &name);
    void unloadPlugin();

    QScopedPointer<PluginManagerPrivate> d_ptr;
};

} // namespace RestLink

#endif // RESTLINK_PLUGINMANAGER_H
