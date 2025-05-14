#ifndef DEBUG_H
#define DEBUG_H

#include <QtCore/qloggingcategory.h>

#define sqlDebug()    qCDebug(restlinkSql).noquote().nospace()
#define sqlInfo()     qCInfo(restlinkSql).noquote().nospace()
#define sqlWarning()  qCWarning(restlinkSql).noquote().nospace()
#define sqlCritical() qCCritical(restlinkSql).noquote().nospace()

Q_DECLARE_LOGGING_CATEGORY(restlinkSql)

#endif // DEBUG_H
