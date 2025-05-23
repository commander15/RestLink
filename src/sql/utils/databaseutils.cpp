#include "databaseutils.h"

#include <api.h>

#include <qsqlindex.h>

namespace RestLink {
namespace Sql {

QString DatabaseUtils::primaryKeyOn(const QString &tableName, Api *api)
{
    QSqlDatabase db = api->database();

    const QSqlIndex index = db.primaryIndex(tableName);
    if (index.count() == 1)
        return index.fieldName(0);

    const QSqlRecord record = db.record(tableName);
    return (record.count() > 0 ? record.fieldName(0) : QStringLiteral("id"));
}

QStringList DatabaseUtils::foreignKeysOn(const QString &tableName, Api *api)
{
    QStringList keys;

    const QSqlRecord record = api->database().record(tableName);
    for (int i = 0; i < record.count(); ++i) {
        const QString field = record.fieldName(i);
        if (field.endsWith("_id"))
            keys.append(field);
    }

    return keys;
}

QString DatabaseUtils::foreignKeyFor(const QString &tableName, Api *api)
{
    return singularise(tableName).toLower() + '_' + primaryKeyOn(tableName, api);
}

QString DatabaseUtils::singularise(const QString &tableName)
{
    QString name = tableName;

    if (name.endsWith("ies")) {
        name.remove(name.length() - 3, 3);
        name.append('y');
    } else if (name.endsWith('s')) {
        name.removeLast();
    }

    return name;
}


} // namespace Sql
} // namespace RestLink
