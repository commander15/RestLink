#ifndef RESTLINK_PLUGINMANAGER_H
#define RESTLINK_PLUGINMANAGER_H

#include <RestLink/global.h>
#include <RestLink/plugin.h>

class QPluginLoader;

namespace RestLink {

class PluginManagerPrivate;
class RESTLINK_EXPORT PluginManager
{
public:
    enum LoadMode {
        SafeMode,
        UnsafeMode,
    };

    ~PluginManager();

    static bool loadPlugin(const QString &name, LoadMode mode = SafeMode);
    static QStringList loadAvailablePlugins(LoadMode mode = SafeMode);
    static QStringList discoverPlugins();

private:
    PluginManager();

    static PluginManagerPrivate *internal();

    QScopedPointer<PluginManagerPrivate> d_ptr;
};

} // namespace RestLink

#endif // RESTLINK_PLUGINMANAGER_H
