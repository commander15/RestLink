#ifndef QUERYRUNNER_H
#define QUERYRUNNER_H

#include <QtCore/qjsonobject.h>

class QSqlQuery;

namespace RestLink {
namespace Sql {

class Api;

class QueryRunner
{
public:
    //static QSqlQuery exec(const QString &statement, QJsonObject *error = nullptr);
};

} // namespace Sql
} // namespace RestLink

#endif // QUERYRUNNER_H
