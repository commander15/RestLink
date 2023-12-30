#include "restlinkplugin.h"

#include <RestLink/api.h>
#include <RestLink/apireply.h>

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

    qmlRegisterType<RestLink::Api>(uri, 1, 0, "RestLinkApi");

    qmlRegisterUncreatableType<RestLink::ApiReply>(uri, 1, 0, "RestLinkApiReply",
                                                   tr("Need to be instanctiate by RestLink::Api"));
}
