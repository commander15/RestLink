#ifndef RESTLINK_DEBUG_H
#define RESTLINK_DEBUG_H

#include <RestLink/global.h>

#include <QtCore/qloggingcategory.h>

#define restlinkDebug()    qCDebug(restlink).noquote().nospace()
#define restlinkInfo()     qCInfo(restlink).noquote().nospace()
#define restlinkWarning()  qCWarning(restlink).noquote().nospace()
#define restlinkCritical() qCCritical(restlink).noquote().nospace()

RESTLINK_EXPORT Q_DECLARE_LOGGING_CATEGORY(restlink)

#endif // RESTLINK_DEBUG_H
