#ifndef GLOBAL_H
#define GLOBAL_H

#include <RestLink/global.h>

#ifndef RESTLINK_DEBUG
#   define SQL_EXPORT Q_DECL_HIDDEN
#else
#   define SQL_EXPORT
#endif

#endif // GLOBAL_H
