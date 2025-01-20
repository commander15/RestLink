#ifndef RESTLINK_PLUGIN_H
#define RESTLINK_PLUGIN_H

#include <RestLink/global.h>

#include <QtCore/qobject.h>

#define RESTLINK_PLUGIN_IID "com.restlink.plugin"

namespace RestLink {

class RequestHandler;

class RESTLINK_EXPORT Plugin : public QObject
{
    Q_OBJECT

public:
    explicit Plugin(QObject *parent = nullptr);
    virtual ~Plugin() = default;

    virtual RequestHandler *createHandler() = 0;
    static QList<RequestHandler *> allHandlers();

    static bool isDiscoveryEnabled();
    static void enableDiscovery();
    static void setDiscoveryEnabled(bool enable = true);

private:
    static QStringList pluginNames();

    static bool s_discoveryEnabled;
    static QList<RequestHandler *> s_handlers;
};

} // namespace RestLink

Q_DECLARE_INTERFACE(RestLink::Plugin, RESTLINK_PLUGIN_IID)

#endif // RESTLINK_PLUGIN_H
