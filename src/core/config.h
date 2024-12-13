#ifndef RESTLINK_CONFIG_H
#define RESTLINK_CONFIG_H

#include <QtCore/qglobal.h>

#define RESTLINK_VERSION_MAJOR 1
#define RESTLINK_VERSION_MINOR 1
#define RESTLINK_VERSION_PATCH 0
#define RESTLINK_VERSION_STR   "1.1.0"
#define RESTLINK_VERSION       RESTLINK_VERSION_CHECK(RESTLINK_VERSION_MAJOR, RESTLINK_VERSION_MINOR, RESTLINK_VERSION_PATCH)
#define RESTLINK_VERSION_CHECK QT_VERSION_CHECK

#define RESTLINK_DEBUG QT_DEBUG

#endif // RESTLINK_CONFIG_H
