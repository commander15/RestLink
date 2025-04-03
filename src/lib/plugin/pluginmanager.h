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

    static QList<RequestHandler *> handlers();

    static bool isDiscoveryEnabled();
    static void enableDiscovery();
    static void setDiscoveryEnabled(bool enable = true);

    static void registerPlugin(const QString &name);

    RequestHandler *createHandler(Plugin *plugin);
    Plugin *loadPlugin(const QString &name);
    void unloadPlugin();

    static PluginManager *global();

private:
    QScopedPointer<PluginManagerPrivate> d_ptr;
};

} // namespace RestLink

#endif // RESTLINK_PLUGINMANAGER_H
