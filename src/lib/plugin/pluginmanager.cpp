#include "pluginmanager.h"
#include "pluginmanager_p.h"

#include <RestLink/debug.h>
#include <RestLink/abstractrequesthandler.h>
#include <RestLink/networkmanager.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qdiriterator.h>
#include <QtCore/qdir.h>
#include <qstandardpaths.h>

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
    NetworkManager::loadNetworkSchemes();
}

/**
 * @brief Destroys the PluginManager instance.
 */
PluginManager::~PluginManager()
{
}

QList<Plugin *> PluginManager::loadedPlugins()
{
    return internal()->loadedPlugins;
}

/**
 * @brief Load a RestLink plugin according to mode policy.
 * @return True if the plugin has been loaded, False otherwise
 */
bool PluginManager::loadPlugin(const QString &name, LoadMode mode)
{
    PluginLoadHelper helper;
    PluginManagerPrivate *d = internal();

    // We try to load the plugin
    QString errorString;
    Plugin *plugin = helper.load(name, errorString);

    // If load failed, we just add a warning and return false
    if (!plugin) {
        restlinkWarning() << errorString;
        return false;
    }

    // If the plugin is already loaded, we do nothing
    auto it = std::find_if(d->loadedPlugins.cbegin(), d->loadedPlugins.cend(), [plugin](Plugin *loaded) {
        return loaded->uuid() == plugin->uuid();
    });
    if (it != d->loadedPlugins.cend())
        return true;

    // We create an handler from the plugin
    AbstractRequestHandler *handler = helper.handler();

    // If we can't create a handle, we issue a warning with the error that occured
    if (!handler) {
        restlinkWarning() << errorString;
        return false;
    }

    // We retrieve supported schemes
    const QStringList handlerSchemes = handler->supportedSchemes();

    // We block HTTP/HTTPS plugins
    if (handlerSchemes.contains("http", Qt::CaseInsensitive)) {
        restlinkWarning() << "an HTTP/HTTPS handler has been detected on '" << plugin->name()
        << "' plugin, this is unsuported for security reasons";
        return false;
    }

    // We try to register the handler
    switch (NetworkManager::registerHandler(handler)) {
    case NetworkManager::InvalidHandlerRegistrationError:
        restlinkWarning() << "can't register plugin " + name + ", NetworkManager refuses it, may be it's a duplicate";
        return false;

    case NetworkManager::HandlerAlreadyRegisteredError:
        restlinkWarning() << "can't register plugin " + name + ", NetworkManager refuses it, may be it's a duplicate";
        return false;

    case NetworkManager::HandlerSchemesAlreadyExistsError:
        restlinkWarning() << "can't register plugin " + name + ", NetworkManager refuses it, may be it's a duplicate";
        return false;

    case NetworkManager::NoHandlerRegistrationError:
        break;
    }

    helper.commit();
    d->loadedPlugins.append(plugin);
    restlinkInfo() << plugin->name() << " plugin version " << plugin->version()
                   << " loaded, supported schemes: " << handlerSchemes.join(", ");
    return true;
}

/**
 * @brief Discover and load all available plugins according to mode policies.
 * @return The list of loaded plugins.
 */
QStringList PluginManager::loadAvailablePlugins(LoadMode mode)
{
    QStringList plugins = discoverPlugins();
    for (const QString &p : std::as_const(plugins)) {
        if (!loadPlugin(p, mode))
            plugins.removeOne(p);
    }
    return plugins;
}

/**
 * @brief Find all available plugins.
 * @return The plugins list.
 * @note The plugin names returned by this method may refers to invalid or insecure plugins.
 */
QStringList PluginManager::discoverPlugins()
{
    static const QStringList pluginsDirs = qApp->libraryPaths();

    QStringList plugins;
    for (const QString &pluginsDir : pluginsDirs) {
        QDirIterator it(pluginsDir, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            const QString file = it.next();
            if (file.contains("restlink", Qt::CaseInsensitive) && QLibrary::isLibrary(file))
                plugins.append(file);
        }
    }

    return plugins;
}

PluginManagerPrivate *PluginManager::internal()
{
    static QScopedPointer<PluginManager> manager;
    if (!manager)
        manager.reset(new PluginManager);
    return manager->d_ptr.get();
}

PluginManagerPrivate::~PluginManagerPrivate()
{
    while (!loadedPlugins.isEmpty())
        delete loadedPlugins.takeFirst();
}

AbstractRequestHandler *PluginManagerPrivate::createHandler(Plugin *plugin, QString &errorString)
{
    AbstractRequestHandler *handler = plugin->createHandler(qApp);

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

PluginLoadHelper::PluginLoadHelper()
    : m_plugin(nullptr)
    , m_handler(nullptr)
    , m_commit(false) {
    // If a plugin is already loaded, we unload it first
    if (m_loader.isLoaded())
        m_loader.unload();
}

PluginLoadHelper::~PluginLoadHelper()
{
    if (m_commit)
        return;

    // If no commit, we free memory

    if (m_loader.isLoaded())
        m_loader.unload();

    if (m_handler != nullptr)
        delete m_handler;
}

Plugin *PluginLoadHelper::load(const QString &name, QString &errorString)
{
    m_loader.setFileName(name);

    // If we can't load it, we can't go further
    if (!m_loader.load()) {
        errorString = "Failed to load plugin: " + name + ' ' + m_loader.errorString();
        return nullptr;
    }

    // Loading meta data
    const QJsonObject metaData = m_loader.metaData();
    const QString pluginId = metaData.value("IID").toString();
    if (pluginId.isEmpty()) {
        const QString name = metaData.value("name").toString();
        errorString = "A plugin without IID has been detected and can't be loaded"
                      + (name.isEmpty() ? "" : ", reported its name as: " + name);

        m_loader.unload();
        return nullptr;
    }

    // loading plugin
    m_plugin = reinterpret_cast<Plugin *>(m_loader.instance());
    if (!m_plugin) {
        errorString = "Invalid plugin interface for: " + name;

        m_loader.unload();
        return nullptr;
    }

    m_plugin->setMetaData(metaData.value("MetaData").toObject());

    // We can't identify ? Right, just skip it
    if (m_plugin->name().isEmpty()) {
        errorString = "suspect plugin detected: " + m_loader.fileName();

        m_loader.unload();
        return nullptr;
    }

    return m_plugin;
}

AbstractRequestHandler *PluginLoadHelper::handler()
{
    if (m_handler == nullptr && m_plugin != nullptr)
        m_handler = m_plugin->createHandler(qApp);
    return m_handler;
}

} // namespace RestLink
