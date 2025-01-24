#include "excelplugin.h"

#include "excelserver.h"

namespace RestLink {

ExcelPlugin::ExcelPlugin(QObject *parent)
    : Plugin(parent)
{
}

RequestHandler *ExcelPlugin::createHandler()
{
    return new ExcelServer();
}

} // namespace RestLink
