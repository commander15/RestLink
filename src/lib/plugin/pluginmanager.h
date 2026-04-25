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

    static QList<Plugin *> loadedPlugins();
    static bool loadPlugin(const QString &name, LoadMode mode = SafeMode);
    static QStringList loadAvailablePlugins(LoadMode mode = SafeMode);
    static QStringList discoverPlugins();

private:
    PluginManager();
    ~PluginManager();

    static PluginManagerPrivate *internal();

    QScopedPointer<PluginManagerPrivate> d_ptr;

    friend class QScopedPointerDeleter<PluginManager>;
};

} // namespace RestLink

#endif // RESTLINK_PLUGINMANAGER_H
