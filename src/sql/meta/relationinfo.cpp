#include "relationinfo.h"

#include "resourceinfo.h"

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

    auto generateLocalKey = [&resource](const QJsonObject &relation) -> QString {
        QString table = relation.value("table").toString();
        table = table.toLower();
        if (table.endsWith('s'))
            table.removeLast();

        switch (Relation::typeFromString(relation.value("type").toString())) {
        case Relation::HasOne:
        case Relation::HasMany:
            return resource.primaryKey();

        case Relation::HasManyThrough:
            return "id";

        case Relation::BelongsToOne:
        case Relation::BelongsToManyThrough:
            return table + "_id";

        case Relation::BelongsToMany:
            return resource.localKey();

        default:
            return QString();
        }
    };

    auto generateForeignKey = [&resource](const QJsonObject &relation) -> QString {
        QString table = relation.value("table").toString();
        table = table.toLower();
        if (table.endsWith('s'))
            table.removeLast();

        switch (Relation::typeFromString(relation.value("type").toString())) {
        case Relation::HasOne:
        case Relation::HasMany:
        case Relation::HasManyThrough:
        case Relation::BelongsToManyThrough:
            return resource.localKey();

        case Relation::BelongsToOne:
            return "id";

        case Relation::BelongsToMany:
            return table + "_id";

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
    attribute("owned", false, &d->owned);
    attribute("auto_load", false, &d->autoLoadable);
    attribute("nested_load", false, &d->nestedLoadable);
    endParsing();

    if (!d->intermediate.isEmpty())
        d->intermediateRecord = api->database().record(d->intermediate);
}

void RelationInfo::save(QJsonObject *object) const
{
    object->insert("table", d->table);
    object->insert("intermediate", d->intermediate);
    object->insert("pivot", d->pivot);
    object->insert("local_key", d->localKey);
    object->insert("foreign_key", d->foreignKey);
    object->insert("owned", d->owned);
    object->insert("auto_load", d->autoLoadable);
    object->insert("nested_load", d->nestedLoadable);
    object->insert("type", Relation::stringFromType(d->type));
}

} // namespace Sql
} // namespace RestLink
