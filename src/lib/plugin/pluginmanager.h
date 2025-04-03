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

    QList<RequestHandler *> handlers();

    bool isDiscoveryEnabled();
    void enableDiscovery();
    void setDiscoveryEnabled(bool enable = true);

    void registerPlugin(const QString &name);

    RequestHandler *createHandler(Plugin *plugin);
    Plugin *loadPlugin(const QString &name);
    QList<Plugin *> loadPlugins(const QStringList &names);

    static PluginManager *global();

private:
    QScopedPointer<PluginManagerPrivate> d_ptr;
};

} // namespace RestLink

#endif // RESTLINK_PLUGINMANAGER_H
