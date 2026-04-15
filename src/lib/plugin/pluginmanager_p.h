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
    AbstractRequestHandler *createHandler(Plugin *plugin, QString &errorString);

    Plugin *load(const QString &name, QString &errorString);
    bool unload();

    QStringList loadedPlugins;

private:
    QPluginLoader m_loader;
};

}

#endif // RESTLINK_PLUGINMANAGER_P_H
