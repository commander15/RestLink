#ifndef RESTLINK_PLUGINMANAGER_P_H
#define RESTLINK_PLUGINMANAGER_P_H

#include <QtCore/qlist.h>
#include <QtCore/qpluginloader.h>

namespace RestLink {

class AbstractRequestHandler;

class PluginManagerPrivate
{
public:
    QStringList names;
    bool discoveryEnabled = false;
    QList<AbstractRequestHandler *> handlers;

    QPluginLoader pluginLoader;
};

}

#endif // RESTLINK_PLUGINMANAGER_P_H
