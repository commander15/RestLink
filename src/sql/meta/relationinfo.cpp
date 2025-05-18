#include "relationinfo.h"

#include "resourceinfo.h"

#include <api.h>
#include <data/relation.h>
#include <data/model.h>

#include <QtCore/qjsonobject.h>

namespace RestLink {
namespace Sql {

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
    return !d->table.isEmpty();
}

int RelationInfo::type() const
{
    return d->type;
}

void RelationInfo::load(const QString &name, const QJsonObject &object, const ResourceInfo &resource, Api *api)
{
    d->name = name;

    beginParsing(object);

    auto generateLocalKey = [&resource](const QJsonObject &) -> QString {
        return resource.foreignKey();
    };

    auto generateForeignKey = [&api, this](const QJsonObject &) -> QString {
        const QString table = d->table;

        QString prefix = table;
        if (prefix.endsWith('s'))
            prefix.removeLast();

        QString id = api->resourceInfoByTable(table).primaryKey();

        return table.toLower() + '_' + id;
    };

    attribute("table", &d->table);
    attribute("pivot", &d->pivot);
    attribute("local_key", Callback<QString>(generateLocalKey), &d->localKey);
    attribute("foreign_key", Callback<QString>(generateForeignKey), &d->foreignKey);
    attribute("owned", false, &d->owned);
    attribute("auto_load", false, &d->autoLoadable);
    attribute("nested_load", false, &d->nestedLoadable);

    const QString type = object.value("type").toString();
    if (type == "HasOne")
        d->type = static_cast<int>(Relation::Type::HasOne);
    else if (type == "BelongsTo")
        d->type = static_cast<int>(Relation::Type::BelongsToOne);
    else if (type == "HasMany")
        d->type = static_cast<int>(Relation::Type::HasMany);
    else if (type == "BelongsToMany")
        d->type = static_cast<int>(Relation::Type::BelongsToMany);
    else if (type == "HasManyThrough")
        d->type = static_cast<Relation::Type>(Relation::Type::HasManyThrough);
    else if (type == "BelongsToManyThrough")
        d->type = static_cast<Relation::Type>(Relation::Type::BelongsToManyThrough);
    else
        d->type = static_cast<int>(Relation::Type::Null);

    endParsing();
}

void RelationInfo::save(QJsonObject *object) const
{
    object->insert("table", d->table);
    object->insert("pivot", d->pivot);
    object->insert("local_key", d->localKey);
    object->insert("foreign_key", d->foreignKey);
    object->insert("owned", d->owned);
    object->insert("auto_load", d->autoLoadable);
    object->insert("nested_load", d->nestedLoadable);

    switch (static_cast<Relation::Type>(d->type)) {
        case Relation::Type::HasOne:
        object->insert("type", "HasOne");
            break;

        case Relation::Type::BelongsToOne:
            object->insert("type", "BelongsToOne");
            break;

        case Relation::Type::HasMany:
            object->insert("type", "HasMany");
            break;

        case Relation::Type::BelongsToMany:
            object->insert("type", "BelongsToMany");
            break;

        case Relation::Type::HasManyThrough:
            object->insert("type", "HasManyThrough");
            break;

        case Relation::Type::BelongsToManyThrough:
            object->insert("type", "BelongsToManyThrough");
            break;

        default:
            object->insert("type", "Unknown");
            break;
        }
}

} // namespace Sql
} // namespace RestLink
