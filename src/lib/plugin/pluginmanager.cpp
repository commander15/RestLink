#include "pluginmanager.h"
#include "pluginmanager_p.h"

#include <RestLink/debug.h>
#include <RestLink/abstractrequesthandler.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>
#include <QtCore/qlibrary.h>

namespace RestLink {

/**
 * @class PluginManager
 * @brief Manages discovery, loading, and instantiation of RestLink plugins.
 *
 * This class provides a central interface for registering and loading plugins that extend
 * RestLink's request handling capabilities. It supports optional plugin discovery, which scans
 * runtime library paths for valid RestLink plugins.
 *
 * @warning Plugin discovery is disabled by default due to security concerns: allowing arbitrary shared libraries
 * to be loaded from library paths can introduce vulnerabilities. Enabling discovery should be done
 * with caution and only in trusted environments, use enableDiscovery() to explicitly enable discovery,
 * alternatively, use the static registerPlugin() method to securely specify which plugins should be loaded by name,
 * bypassing the need for unrestricted path scanning.
 */

/**
 * @brief Constructs a new PluginManager instance.
 */
PluginManager::PluginManager()
    : d_ptr(new PluginManagerPrivate)
{
}

/**
 * @brief Destroys the PluginManager instance.
 */
PluginManager::~PluginManager()
{
}

/**
 * @brief Returns a list of all discovered and valid AbstractRequestHandler instances.
 * @return QList of AbstractRequestHandler pointers.
 */
QList<AbstractRequestHandler *> PluginManager::handlers()
{
    static QList<AbstractRequestHandler *> handlers;

    if (!handlers.isEmpty())
        return handlers;

    QStringList pluginNames;
    QPluginLoader loader;

    PluginManager *manager = global();
    PluginManagerPrivate *data = manager->d_ptr.get();

    if (data->discoveryEnabled) {
        const QStringList searchPaths = QCoreApplication::libraryPaths();

        for (const QString &path : searchPaths) {
            QDir dir(path + "/restlink");

            const QStringList files = dir.entryList(QDir::Files);

            for (const QString &file : files) {
                // loading plugin
                Plugin *plugin = manager->loadPlugin(dir.filePath(file));
                if (!plugin)
                    continue;

                // Retrieve handler
                AbstractRequestHandler *handler = manager->createHandler(plugin);

                if (handler) {
                    pluginNames.append(plugin->name());
                    handlers.append(handler);
                }

                manager->unloadPlugin();
            }
        }
    }

    return handlers;
}

/**
 * @brief Returns whether plugin discovery is currently enabled.
 * @return true if discovery is enabled, false otherwise.
 */
bool PluginManager::isDiscoveryEnabled()
{
    return global()->d_ptr->discoveryEnabled;
}

/**
 * @brief Enables plugin discovery.
 *
 * When enabled, RestLink will scan library paths at runtime to detect and load available plugins.
 * This feature is disabled by default to avoid potential security risks from untrusted binaries.
 *
 * @note This must be called before creating any NetworkManager instance.
 * It is recommended to invoke this immediately after initializing the Qt application object
 * in your main application file.
 */
void PluginManager::enableDiscovery()
{
    global()->d_ptr->discoveryEnabled = true;
}

/**
 * @brief Sets whether plugin discovery should be enabled.
 * @param enable If true, enables discovery; disables it otherwise.
 */
void PluginManager::setDiscoveryEnabled(bool enable)
{
    global()->d_ptr->discoveryEnabled = enable;
}

/**
 * @brief Registers a plugin name manually.
 *
 * This will cause the plugin manager attempt to load a plugin,
 * should be more secure than plugin discovery.
 *
 * @param name The name of the plugin to load.
 */
void PluginManager::registerPlugin(const QString &name)
{
    QStringList *names = &global()->d_ptr->names;
    if (!names->contains(name))
        names->append(name);
}

/**
 * @brief Creates a handler instance from a plugin.
 * @param plugin The plugin to instantiate a handler from.
 * @return A pointer to an AbstractRequestHandler, or nullptr if failed.
 */
AbstractRequestHandler *PluginManager::createHandler(Plugin *plugin)
{
    AbstractRequestHandler *handler = plugin->createHandler();

    if (!handler) {
        restlinkWarning() << "failed to create handler for plugin: " << plugin->name();
        return nullptr;
    }

    const QStringList schemes = handler->supportedSchemes();
    if (schemes.contains("http", Qt::CaseInsensitive) || schemes.contains("https", Qt::CaseInsensitive)) {
        restlinkWarning() << "an HTTP/HTTPS handler has been detected on '" << plugin->name()
        << "' plugin, this is unsuported for security reasons";
        delete handler;
        return nullptr;
    }

    restlinkInfo() << plugin->name() << " plugin loaded, supported schemes: "
                   << schemes.join(", ");
    return handler;
}

/**
 * @brief Loads a plugin from the given name or absolute path.
 * @param name The plugin file name or absolute path.
 * @return A pointer to the loaded Plugin, or nullptr on failure.
 */
Plugin *PluginManager::loadPlugin(const QString &name)
{
    unloadPlugin();

    if (QDir::isAbsolutePath(name))
        d_ptr->pluginLoader.setFileName(name);
    else
        d_ptr->pluginLoader.setFileName("restlink/" + name);

    if (!d_ptr->pluginLoader.load()) {
        restlinkWarning() << "Failed to load plugin: " << name << d_ptr->pluginLoader.errorString();
        return nullptr;
    }

    // Loading meta data
    const QJsonObject metaData = d_ptr->pluginLoader.metaData();
    if (!metaData.value("IID").toString().startsWith("com.restlink.")) {
        restlinkWarning() << "invalid plugin";
        unloadPlugin();
        return nullptr;
    }

    // loading plugin
    Plugin *plugin = reinterpret_cast<Plugin *>(d_ptr->pluginLoader.instance());
    if (!plugin) {
        restlinkWarning() << "Invalid plugin interface for:" << name;
        unloadPlugin();
        return nullptr;
    }

    plugin->setMetaData(metaData.value("MetaData").toObject());

    // We can't identify ? Right, just skip it
    if (plugin->name().isEmpty()) {
        restlinkWarning() << "suspect plugin detected: " << d_ptr->pluginLoader.fileName();
        return nullptr;
    }

    return plugin;
}

/**
 * @brief Unloads the currently loaded plugin.
 */
void PluginManager::unloadPlugin()
{
    if (d_ptr->pluginLoader.isLoaded())
        d_ptr->pluginLoader.unload();
}

/**
 * @brief Returns the global singleton instance of PluginManager.
 * @return A pointer to the global PluginManager.
 */
PluginManager *PluginManager::global()
{
    static PluginManager manager;
    return &manager;
}

} // namespace RestLink
