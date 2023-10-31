#include "restlinkplugin.h"

#include "restlinkapi.h"
#include "restlinkapirequest.h"

#include <QtQml/qqmlengine.h>

RestLinkPlugin::RestLinkPlugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

void RestLinkPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<RestLinkApi>(uri, 1, 0, "RestLinkApi");
    qmlRegisterType<RestLinkApiRequest>(uri, 1, 0, "RestLinkApiRequest");
    qmlRegisterType<RestLinkApiRequestParameter>(uri, 1, 0, "RestLinkApiRequestParameter");
    qmlRegisterUncreatableType<RestLink::ApiReply>(uri, 1, 0, "RestLinkApiReply", "");
}
