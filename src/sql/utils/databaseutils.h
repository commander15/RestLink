#ifndef DATABASEUTILS_H
#define DATABASEUTILS_H

#include <global.h>

namespace RestLink {
namespace Sql {

class Api;

class SQL_EXPORT DatabaseUtils
{
public:
    static QString primaryKeyOn(const QString &tableName, Api *api);
    static QStringList foreignKeysOn(const QString &tableName, Api *api);

    static QString foreignKeyFor(const QString &tableName, Api *api);

    static QString singularise(const QString &tableName);
};

} // namespace Sql
} // namespace RestLink

#endif // DATABASEUTILS_H
