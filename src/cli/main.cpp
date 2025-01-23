#include "app.h"

#include <QtCore/qloggingcategory.h>

#include <RestLink/plugin.h>
#include <RestLink/api.h>

using namespace RestLink;

int main(int argc, char *argv[])
{
    App app(argc, argv);
    app.setApplicationName("RestCli");
    app.setApplicationVersion(RESTLINK_VERSION_STR);
    app.initParser();

    RestLink::Plugin::enableDiscovery();

    //QLoggingCategory::setFilterRules("restlink.info=true");

    Api api;
    app.setApi(&api);

    return app.exec();
}
