#ifndef RESTLINK_FILEUTILS_H
#define RESTLINK_FILEUTILS_H

#include <RestLink/global.h>

#include <QtCore/qstandardpaths.h>

namespace RestLink {

class RESTLINK_EXPORT FileUtils
{
public:
    static QString generateCacheDir();
    static QString generateCookieFile();
    static QString generateDir(const QString &name, QStandardPaths::StandardLocation location);
};

} // namespace RestLink

#endif // RESTLINK_FILEUTILS_H
