#ifndef RESOURCEINFO_H
#define RESOURCEINFO_H

#include "parseddata.h"

#include <QtCore/qshareddata.h>
#include <QtCore/qlist.h>

class QJsonObject;
class QSqlField;
class QSqlRecord;

namespace RestLink {
namespace Sql {

class RelationInfo;
class Api;

class ResourceInfoData;
class ResourceInfo final : public ParsedData
{
public:
    ResourceInfo();
    ResourceInfo(const ResourceInfo &other);
    ResourceInfo(ResourceInfo &&other);
    ~ResourceInfo();

    ResourceInfo &operator=(const ResourceInfo &);
    ResourceInfo &operator=(ResourceInfo &&);

    QString name() const;
    QString table() const;
    QString primaryKey() const;
    QString foreignKey() const;

    QStringList fillableFields() const;

    bool hasHiddenFields() const;
    QStringList hiddenFields() const;

    QMetaType fieldType(const QString &name) const;
    QSqlField field(const QString &name) const;
    QStringList fieldNames() const;
    QSqlRecord record() const;

    RelationInfo relation(const QString &name) const;
    QStringList relationNames() const;
    QList<RelationInfo> relations() const;

    bool loadRelations() const;

    bool isValid() const;

    void load(const QString &name, const QJsonObject &object, Api *api);
    void save(QJsonObject *object) const;

private:
    QSharedDataPointer<ResourceInfoData> d;
};

} // namespace Sql
} // namespace RestLink

#endif // RESOURCEINFO_H
