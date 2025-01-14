#ifndef RESTLINK_SQLPLUGIN_H
#define RESTLINK_SQLPLUGIN_H

#include <RestLink/plugin.h>

namespace RestLink {

class SqlPlugin : public Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID RESTLINK_PLUGIN_IID FILE "metadata.json")

public:
    explicit SqlPlugin(QObject *parent = nullptr);

    RequestHandler *createHandler() override;
};

}

#endif // RESTLINK_SQLPLUGIN_H
