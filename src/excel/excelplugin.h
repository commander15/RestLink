#ifndef EXCELPLUGIN_H
#define EXCELPLUGIN_H

#include <RestLink/plugin.h>

namespace RestLink {

class ExcelPlugin : public Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID RESTLINK_PLUGIN_IID FILE "metadata.json")

public:
    explicit ExcelPlugin(QObject *parent = nullptr);

    RequestHandler *createHandler() override;
};

} // namespace RestLink

#endif // EXCELPLUGIN_H
