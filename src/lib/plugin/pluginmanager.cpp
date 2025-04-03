#include "pluginmanager.h"
#include "pluginmanager_p.h"

#include <RestLink/debug.h>
#include <RestLink/requesthandler.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>
#include <QtCore/qlibrary.h>

namespace RestLink {

PluginManager::PluginManager()
    : d_ptr(new PluginManagerPrivate)
{
}

PluginManager::~PluginManager()
{
}

QList<RequestHandler *> PluginManager::handlers()
{
    static QList<RequestHandler *> handlers;

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
                RequestHandler *handler = manager->createHandler(plugin);

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

bool PluginManager::isDiscoveryEnabled()
{
    return global()->d_ptr->discoveryEnabled;
}

void PluginManager::enableDiscovery()
{
    global()->d_ptr->discoveryEnabled = true;
}

void PluginManager::setDiscoveryEnabled(bool enable)
{
    global()->d_ptr->discoveryEnabled = enable;
}

void PluginManager::registerPlugin(const QString &name)
{
    QStringList *names = &global()->d_ptr->names;
    if (!names->contains(name))
        names->append(name);
}

RequestHandler *PluginManager::createHandler(Plugin *plugin)
{
    RequestHandler *handler = plugin->createHandler();

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
    if (metaData.value("IID") != RESTLINK_PLUGIN_IID) {
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

void PluginManager::unloadPlugin()
{
    if (d_ptr->pluginLoader.isLoaded())
        d_ptr->pluginLoader.unload();
}

PluginManager *PluginManager::global()
{
    static PluginManager manager;
    return &manager;
}

} // namespace RestLink
