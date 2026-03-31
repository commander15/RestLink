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

    PluginManager::enableDiscovery();

    Api api;
    app.setApi(&api);

    return app.exec();
}
