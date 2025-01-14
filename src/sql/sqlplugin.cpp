#include "sqlplugin.h"

#include "sqlserver.h"

#include <QtCore/qcoreapplication.h>

namespace RestLink {

SqlPlugin::SqlPlugin(QObject *parent)
    : Plugin{parent}
{
}

RequestHandler *SqlPlugin::createHandler()
{
    return new SqlServer(QCoreApplication::instance());
}

}
