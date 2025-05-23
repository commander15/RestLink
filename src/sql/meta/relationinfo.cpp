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
    QString localKey;
    QString foreignKey;
    QSqlRecord intermediateRecord;
    QStringList loadableRelations;
    bool owned = false;
    bool autoLoadable = false;
    bool nestedLoadable = false;
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

QString RelationInfo::localKey() const
{
    return d->localKey;
}

QString RelationInfo::foreignKey() const
{
    return d->foreignKey;
}

QStringList RelationInfo::loadableRelations() const
{
    return d->loadableRelations;
}

bool RelationInfo::owned() const
{
    return d->owned;
}

bool RelationInfo::autoLoadable() const
{
    return d->autoLoadable;
}

bool RelationInfo::nestLoadable() const
{
    return d->nestedLoadable;
}

bool RelationInfo::isValid() const
{
    return !d->table.isEmpty()
           || !d->localKey.isEmpty()
           || !d->foreignKey.isEmpty()
           || d->type != Relation::Null;
}

int RelationInfo::type() const
{
    return d->type;
}

void RelationInfo::load(const QString &name, const QJsonObject &object, const ResourceInfo &resource, Api *api)
{
    d->name = name;
    d->type = Relation::typeFromString(object.value("type").toString());

    auto generateLocalKey = [&resource, &api](const QJsonObject &relation) -> QString {
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
    attribute("with", &d->loadableRelations);

    beginParsing(object.value("load").toObject());
    attribute("auto", false, &d->autoLoadable);
    attribute("nest", false, &d->nestedLoadable);
    endParsing();

    endParsing();

    if (!d->intermediate.isEmpty())
        d->intermediateRecord = api->database().record(d->intermediate);
}

void RelationInfo::save(QJsonObject *object) const
{
    object->insert("table", d->table);
    object->insert("intermediate", d->intermediate);
    object->insert("with", QJsonValue::fromVariant(d->loadableRelations));
    object->insert("pivot", d->pivot);
    object->insert("local_key", d->localKey);
    object->insert("foreign_key", d->foreignKey);
    object->insert("owned", d->owned);

    QJsonObject load;
    load.insert("auto", d->autoLoadable);
    load.insert("nest", d->nestedLoadable);
    object->insert("load", load);

    object->insert("type", Relation::stringFromType(d->type));
}

} // namespace Sql
} // namespace RestLink
