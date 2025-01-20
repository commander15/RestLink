#include "restlinkplugin.h"

#include <RestLink/response.h>
#include <RestLink/cache.h>
#include <RestLink/cookiejar.h>

#include <QtQml/qqmlengine.h>

extern void qml_register_types_RestLink();

RestLinkPlugin::RestLinkPlugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

void RestLinkPlugin::registerTypes(const char *uri)
{
    qml_register_types_RestLink();

    // @uri RestLink

    qmlRegisterUncreatableType<RestLink::Response>(uri, 1, 0, "Response",
                                                   "Need to be instanctiate by RestLink::Api");

    qmlRegisterType<RestLink::Cache>(uri, 1, 0, "Cache");
    qmlRegisterType<RestLink::CookieJar>(uri, 1, 0, "CookieJar");
}
