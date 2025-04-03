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

    if (d_ptr->discoveryEnabled) {
        const QStringList searchPaths = QCoreApplication::libraryPaths();

        for (const QString &path : searchPaths) {
            QDir dir(path + "/restlink");

            const QStringList files = dir.entryList(QDir::Files);

            for (const QString &file : files) {
                // loading plugin
                Plugin *plugin = loadPlugin(dir.filePath(file));
                if (!plugin)
                    continue;

                // Retrieve handler
                RequestHandler *handler = createHandler(plugin);

                if (handler) {
                    pluginNames.append(plugin->name());
                    handlers.append(handler);
                }

                loader.unload();
            }
        }
    }

    return handlers;
}

bool PluginManager::isDiscoveryEnabled()
{
    return d_ptr->discoveryEnabled;
}

void PluginManager::enableDiscovery()
{
    d_ptr->discoveryEnabled = true;
}

void PluginManager::setDiscoveryEnabled(bool enable)
{
    d_ptr->discoveryEnabled = enable;
}

void PluginManager::registerPlugin(const QString &name)
{
    if (!d_ptr->names.contains(name))
        d_ptr->names.append(name);
}

RequestHandler *PluginManager::createHandler(Plugin *plugin)
{
    RequestHandler *handler = plugin->createHandler();

    if (!handler) {
        restlinkWarning() << "failed to create handler for plugin: " << plugin->name();
        return nullptr;
    }

    const QStringList schemes = handler->supportedSchemes();
    if (schemes.contains("http", Qt::CaseInsensitive)) {
        restlinkWarning() << "an HTTP/HTTPS handler has been detected on '" << plugin->name()
                          << "' plugin, this is unsuported for security reasons";
        delete handler;
        return nullptr;
    }

    restlinkWarning() << "failed to create handler for plugin: " << plugin->name();
    return nullptr;
}

Plugin *PluginManager::loadPlugin(const QString &name)
{
    const QList<Plugin *> plugins = loadPlugins({ name });
    return (!plugins.isEmpty() ? plugins.first() : nullptr);
}

QList<Plugin *> PluginManager::loadPlugins(const QStringList &names)
{
    QList<Plugin *> plugins;

    for (const QString &name : names) {
        // Construct the plugin path (could be dynamic)
        d_ptr->pluginLoader.setFileName("restlink/" + name);
        if (!d_ptr->pluginLoader.load()) {
            qWarning() << "Failed to load plugin:" << name << d_ptr->pluginLoader.errorString();
            continue;
        }

        // Loading meta data
        const QJsonObject metaData = d_ptr->pluginLoader.metaData();
        if (metaData.value("IID") != RESTLINK_PLUGIN_IID) {
            qWarning() << "invalid plugin";
            d_ptr->pluginLoader.unload();
            continue;
        }

        // loading plugin
        Plugin *plugin = reinterpret_cast<Plugin *>(d_ptr->pluginLoader.instance());
        if (!plugin) {
            qWarning() << "Invalid plugin interface for:" << name;
            d_ptr->pluginLoader.unload();
            continue;
        }

        plugin->setMetaData(metaData.value("MetaData").toObject());
        plugins.append(plugin);
    }

    return plugins;
}

PluginManager *PluginManager::global()
{
    static PluginManager manager;
    return &manager;
}

} // namespace RestLink
