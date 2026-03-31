#include "relationinfo.h"

#include "resourceinfo.h"
#include "utils/databaseutils.h"

#include <api.h>
#include <data/relation.h>
#include <data/model.h>

#include <QtCore/qjsonobject.h>

#include <QtSql/qsqlrecord.h>

namespace RestLink {
namespace Sql {

class RelationInfoData : public QSharedData {
public:
    QString name;
    QString table;
    QString intermediate;
    QString pivot;
    QString foreignKey;
    QString localKey;
    QSqlRecord intermediateRecord;
    QStringList with;
    Relation::Type type = Relation::Null;
};

RelationInfo::RelationInfo()
    : d(new RelationInfoData)
{
}

RelationInfo::RelationInfo(const RelationInfo &other) = default;

RelationInfo::~RelationInfo() = default;

RelationInfo &RelationInfo::operator=(const RelationInfo &other) = default;

QString RelationInfo::name() const
{
    return d->name;
}

QString RelationInfo::table() const
{
    return d->table;
}

QString RelationInfo::pivot() const
{
    return d->pivot;
}

QString RelationInfo::foreignKey() const
{
    return d->foreignKey;
}

QString RelationInfo::localKey() const
{
    return d->localKey;
}

QString RelationInfo::ownerKey() const
{
    return d->foreignKey;
}

QStringList RelationInfo::with() const
{
    return d->with;
}

int RelationInfo::type() const
{
    return d->type;
}

bool RelationInfo::isValid() const
{
    return !d->table.isEmpty()
           || !d->localKey.isEmpty()
           || !d->foreignKey.isEmpty()
           || d->type != Relation::Null;
}

void RelationInfo::load(const QString &name, const QJsonObject &object, const ResourceInfo &resource, Api *api)
{
    d->name = name;
    d->type = Relation::typeFromString(object.value("type").toString());

    auto generateLocalKey = [&resource, &api](const QJsonObject &relation) -> QString {
        if (relation.contains("owner_key")) {
            const QString ownerKey = relation.value("owner_key").toString();
            if (!ownerKey.isEmpty()) return ownerKey;
        }

        const QString table = relation.value("table").toString();

        switch (Relation::typeFromString(relation.value("type").toString())) {
        case Relation::HasOne:
        case Relation::HasMany:
            return resource.primaryKey();

        case Relation::HasManyThrough:
        case Relation::BelongsToOne:
            return DatabaseUtils::foreignKeyFor(table, api);

        case Relation::BelongsToMany:
        case Relation::BelongsToManyThrough:
            return resource.localKey();

        default:
            return QString();
        }
    };

    auto generateForeignKey = [&resource, &api](const QJsonObject &relation) -> QString {
        const QString table = relation.value("table").toString();

        switch (Relation::typeFromString(relation.value("type").toString())) {
        case Relation::HasOne:
        case Relation::HasMany:
            return resource.localKey();

        case Relation::HasManyThrough:
            return DatabaseUtils::foreignKeyFor(table, api);

        case Relation::BelongsToOne:
            return DatabaseUtils::primaryKeyOn(table, api);

        case Relation::BelongsToMany:
        case Relation::BelongsToManyThrough:
            return DatabaseUtils::foreignKeyFor(table, api);

        default:
            return QString();
        }
    };

    beginParsing(object);
    attribute("table", &d->table);
    attribute("intermediate", &d->intermediate);
    attribute("pivot", &d->pivot);
    attribute("local_key", Callback<QString>(generateLocalKey), &d->localKey);
    attribute("foreign_key", Callback<QString>(generateForeignKey), &d->foreignKey);
    attribute("with", &d->with);
    endParsing();

    if (!d->intermediate.isEmpty())
        d->intermediateRecord = api->database().record(d->intermediate);
}

void RelationInfo::save(QJsonObject *object) const
{
    object->insert("table", d->table);

    if (!d->intermediate.isEmpty())
        object->insert("intermediate", d->intermediate);

    if (!d->pivot.isEmpty())
        object->insert("pivot", d->pivot);

    object->insert("local_key", d->localKey);
    object->insert("foreign_key", d->foreignKey);

    if (!d->with.isEmpty())
        object->insert("with", QJsonValue::fromVariant(d->with));

    object->insert("type", Relation::stringFromType(d->type));
}

} // namespace Sql
} // namespace RestLink
