#include "restlinkplugin.h"

#include <RestLink/api.h>
#include <RestLink/response.h>
#include <RestLink/cache.h>
#include <RestLink/cookiejar.h>

#include <QtQml/qqmlengine.h>

void qml_register_types_RestLink();

RestLinkPlugin::RestLinkPlugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

void RestLinkPlugin::registerTypes(const char *uri)
{
    // @uri RestLink

    int major = 2;

    qmlRegisterUncreatableType<RestLink::Api>(uri, major, 0, "_Api", "");

    qmlRegisterUncreatableType<RestLink::Response>(uri, major, 0, "Response",
                                                   "Need to be instanctiate by RestLink::Api");

    qmlRegisterType<RestLink::Cache>(uri, major, 0, "Cache");
    qmlRegisterType<RestLink::CookieJar>(uri, major, 0, "CookieJar");

    //qml_register_types_RestLink();
}
