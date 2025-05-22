#ifndef RELATIONINFO_H
#define RELATIONINFO_H

#include "parseddata.h"

#include <global.h>

#include <QtCore/qshareddata.h>

class QJsonObject;

namespace RestLink {
namespace Sql {

class ResourceInfo;
class Api;

class RelationInfoData;
class SQL_EXPORT RelationInfo : public ParsedData
{
public:
    RelationInfo();
    RelationInfo(const RelationInfo &other);
    ~RelationInfo();

    RelationInfo &operator=(const RelationInfo &other);

    QString name() const;
    QString table() const;
    QString pivot() const;
    QString localKey() const;
    QString foreignKey() const;

    QStringList loadableRelations() const;

    bool owned() const;
    bool autoLoadable() const;
    bool nestLoadable() const;

    bool isValid() const override;

    int type() const;

    void load(const QString &name, const QJsonObject &object, const ResourceInfo &resource, Api *api);
    void save(QJsonObject *object) const;

private:
    QExplicitlySharedDataPointer<RelationInfoData> d;
};

} // namespace Sql
} // namespace RestLink

#endif // RELATIONINFO_H
