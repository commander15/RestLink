#include "app.h"

#include <QtCore/qloggingcategory.h>

#include <RestLink/pluginmanager.h>
#include <RestLink/api.h>

using namespace RestLink;

int main(int argc, char *argv[])
{
    App app(argc, argv);
    app.setApplicationName("RestCli");
    app.setApplicationVersion(RESTLINK_VERSION_STR);
    app.initParser();

#ifdef QT_DEBUG
    app.addLibraryPath(QString(RESTLINK_ROOT) + "/plugins");
#endif

    // We load all available plugins, no need for extra checks for the tool
    PluginManager::loadAvailablePlugins(PluginManager::UnsafeMode);

    Api api;
    app.setApi(&api);

    return app.exec();
}
