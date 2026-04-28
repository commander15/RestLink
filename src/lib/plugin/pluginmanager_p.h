#ifndef RESTLINK_PLUGINMANAGER_P_H
#define RESTLINK_PLUGINMANAGER_P_H

#include <QtCore/qlist.h>
#include <QtCore/qpluginloader.h>

namespace RestLink {

class AbstractRequestHandler;
class Plugin;

class PluginManagerPrivate
{
public:
    ~PluginManagerPrivate();

    AbstractRequestHandler *createHandler(Plugin *plugin, QString &errorString);

    QList<Plugin *> loadedPlugins;
    QPluginLoader m_loader;
};

class PluginLoadHelper
{
public:
    PluginLoadHelper();
    ~PluginLoadHelper();

    Plugin *load(const QString &name, QString &errorString);

    AbstractRequestHandler *handler();
    void commit() { m_commit = true; }

private:
    QPluginLoader m_loader;
    Plugin *m_plugin;
    AbstractRequestHandler *m_handler;
    bool m_commit;
};

}

#endif // RESTLINK_PLUGINMANAGER_P_H
