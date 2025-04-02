#ifndef SQLDATABASEINFO_H
#define SQLDATABASEINFO_H

#include <QtCore/qjsonobject.h>
#include <QtCore/qstring.h>

namespace RestLink {

class SqlDatabaseInfo
{
public:
    SqlDatabaseInfo();

    QJsonObject configuration;
    QString connection;
};

} // namespace RestLink

#endif // SQLDATABASEINFO_H
