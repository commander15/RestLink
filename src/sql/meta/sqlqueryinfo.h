#ifndef SQLQUERYINFO_H
#define SQLQUERYINFO_H

#include "parseddata.h"

#include <global.h>

#include <QtCore/qshareddata.h>

namespace RestLink {
namespace Sql {

class SqlQueryInfoData;
class SQL_EXPORT SqlQueryInfo : public ParsedData
{
public:
    SqlQueryInfo();
    SqlQueryInfo(const SqlQueryInfo &other);
    ~SqlQueryInfo();

    SqlQueryInfo &operator=(const SqlQueryInfo &other);

    QString query() const;
    QString formated(const QVariantHash &data) const;
    QStringList allFormated(const QVariantHash &data) const;

    bool isObjectQuery() const;
    bool isArrayQuery() const;

    bool isValid() const override;

    void load(const QJsonObject &object);
    void save(QJsonObject *object) const;

private:
    QExplicitlySharedDataPointer<SqlQueryInfoData> d_ptr;
};

} // namespace Sql
} // namespace RestLink

#endif // SQLQUERYINFO_H
