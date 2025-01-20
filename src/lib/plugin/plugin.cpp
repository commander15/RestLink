#include "plugin.h"
#include "plugin_p.h"

#include <RestLink/debug.h>
#include <RestLink/requesthandler.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qpluginloader.h>
#include <QtCore/qdir.h>

namespace RestLink {

Plugin::Plugin(QObject *parent)
    : QObject{parent}
{
}

QList<RequestHandler *> Plugin::allHandlers()
{
    if (!s_handlers.isEmpty())
        return s_handlers;

    QPluginLoader loader;

    // Define plugin names or use a configuration file
    const QStringList pluginNames = Plugin::pluginNames();

    for (const QString &name : pluginNames) {
        // Construct the plugin path (could be dynamic)
        loader.setFileName(name);
        if (!loader.load()) {
            qWarning() << "Failed to load plugin:" << name << loader.errorString();
            continue;
        }

        // Loading meta data
        const QJsonObject metaData = loader.metaData();
        if (metaData.value("IID") != RESTLINK_PLUGIN_IID) {
            qWarning() << "invalid plugin";
            loader.unload();
            continue;
        }

        // loading plugin
        Plugin *plugin = reinterpret_cast<Plugin *>(loader.instance());
        if (!plugin) {
            qWarning() << "Invalid plugin interface for:" << name;
            loader.unload();
            continue;
        }

        const QJsonObject pluginObject = metaData.value("MetaData").toObject();
        const QString pluginName = pluginObject.value("name").toString();

        // Retrieve handler
        RequestHandler *handler = plugin->createHandler();
        if (handler) {
            const QStringList schemes = handler->supportedSchemes();
            const Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive;
            if (schemes.contains("http", sensitivity) && schemes.contains("https", sensitivity)) {
                restlinkWarning() << "an HTTP/HTTPS handler has been detected on '" << pluginName << "' plugin, this is unsuported for security reasons";
                delete handler;
            } else {
                s_handlers.append(handler);
            }
        } else {
            restlinkWarning() << "failed to create handler for plugin: " << pluginName;
        }

        loader.unload();
    }

    return s_handlers;
}

bool Plugin::isDiscoveryEnabled()
{
    return s_discoveryEnabled;
}

void Plugin::enableDiscovery()
{
    s_discoveryEnabled = true;
}

void Plugin::setDiscoveryEnabled(bool enable)
{
    s_discoveryEnabled = enable;
}

QStringList Plugin::pluginNames()
{
    QStringList pluginNames;
    const QStringList searchPaths = QCoreApplication::libraryPaths();

    {
        QStringList names = { "sql" };

        std::transform(names.begin(), names.end(), names.begin(), [&searchPaths](const QString &name) {
            return "restlink/restlink" + name;
        });

        pluginNames.append(names);
    }

    if (isDiscoveryEnabled()) {
        for (const QString &path : searchPaths) {
            QDir dir(path + "/restlink");

            QStringList files = dir.entryList(QDir::Files);

            for (const QString &file : files) {
                const QString pluginName = file.section('.', 0, -2);
                auto it = std::find_if(pluginNames.begin(), pluginNames.end(), [&pluginName](const QString &name) {
                    return name.endsWith(pluginName);
                });

                const QString fileName = dir.filePath(file);
                if (QLibrary::isLibrary(fileName)) {
                    if (it == pluginNames.end())
                        pluginNames.append(fileName);
                    else
                        *it = fileName;
                }
            }
        }
    }

    return pluginNames;
}

bool Plugin::s_discoveryEnabled(false);
QList<RequestHandler *> Plugin::s_handlers;

} // namespace RestLink
