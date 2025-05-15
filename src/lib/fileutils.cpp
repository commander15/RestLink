#include "fileutils.h"

#include <QtCore/qdir.h>
#include <QtCore/qfile.h>

namespace RestLink {

QString FileUtils::generateCacheDir()
{
    static unsigned int index = 0;
    return generateDir(QStringLiteral("Cache%1").arg(index++), QStandardPaths::CacheLocation);
}

QString FileUtils::generateCookieFile()
{
    QString dir = generateDir(QStringLiteral("Cookies"), QStandardPaths::AppLocalDataLocation);

    static unsigned int index = 0;
    QString file = dir + QStringLiteral("/cookies_%1.txt").arg(index++);
    if (!QFile::exists(file)) {
        QFile cookieFile(file);
        cookieFile.open(QIODevice::WriteOnly);
    }

    return file;
}

QString FileUtils::generateDir(const QString &name, QStandardPaths::StandardLocation location)
{
    static const QString rootDirPath = QStandardPaths::writableLocation(location) + "/RestLink";

    const QString dirPath = rootDirPath + '/' + name;
    if (!QFile::exists(dirPath)) {
        QDir dir(dirPath);
        dir.mkpath(".");
    }
    return dirPath;
}

} // namespace RestLink
