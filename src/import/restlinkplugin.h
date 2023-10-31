#ifndef RESTLINKPLUGIN_H
#define RESTLINKPLUGIN_H

#include <QtQml/qqmlextensionplugin.h>

class RestLinkPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.commander.restlink")

public:
    explicit RestLinkPlugin(QObject *parent = nullptr);

    //void initializeEngine(QQmlEngine *engine, const char *uri) override;
    void registerTypes(const char *uri) override;
};

#endif // RESTLINKPLUGIN_H
