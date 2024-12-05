#ifndef RESTLINK_GLOBAL_H
#define RESTLINK_GLOBAL_H

#include <RestLink/config.h>

#ifdef RESTLINK_SHARED
#   ifdef RESTLINK_BUILD
#       define RESTLINK_EXPORT Q_DECL_EXPORT
#   else
#       define RESTLINK_EXPORT Q_DECL_IMPORT
#   endif
#else
#   define RESTLINK_EXPORT
#endif

#define RESTLINK_D(ClassName) ClassName##Private *d = static_cast<ClassName##Private *>(qGetPtrHelper(d_ptr))

#endif // RESTLINK_GLOBAL_H
