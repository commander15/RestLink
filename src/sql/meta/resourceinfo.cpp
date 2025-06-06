#include "resourceinfo.h"
#include "utils/databaseutils.h"

#include <api.h>
#include <meta/relationinfo.h>
#include <utils/querybuilder.h>

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonvalue.h>

#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>
#include <QtSql/qsqlindex.h>

namespace RestLink {
namespace Sql {

class ResourceInfoData : public QSharedData
{
public:
    QString name;
    QString table;
    QString primaryKey;
    QString localKey;
    QStringList fillableProperties;
    QStringList hiddenFields;
    QString createdAtField;
    QString updatedAtField;
    QSqlRecord record;
    QHash<QString, RelationInfo> relations;
    bool loadRelations = false;
};

ResourceInfo::ResourceInfo()
    : d(new ResourceInfoData)
{
}

ResourceInfo::ResourceInfo(const ResourceInfo &other)
    : d{other.d}
{
}

ResourceInfo::ResourceInfo(ResourceInfo &&other)
    : d{std::move(other.d)}
{
}

ResourceInfo::~ResourceInfo()
{
}

ResourceInfo &ResourceInfo::operator=(const ResourceInfo &rhs)
{
    if (this != &rhs)
        d = rhs.d;
    return *this;
}

ResourceInfo &ResourceInfo::operator=(ResourceInfo &&rhs)
{
    if (this != &rhs)
        d = std::move(rhs.d);
    return *this;
}

QString ResourceInfo::name() const
{
    return d->name;
}

QString ResourceInfo::table() const
{
    return d->table;
}

QString ResourceInfo::primaryKey() const
{
    return d->primaryKey;
}

QString ResourceInfo::localKey() const
{
    return d->localKey;
}

bool ResourceInfo::hasCreationTimestamp() const
{
    return !d->createdAtField.isEmpty();
}

QString ResourceInfo::creationTimestampField() const
{
    return d->createdAtField;
}

bool ResourceInfo::hasUpdateTimestamp() const
{
    return !d->updatedAtField.isEmpty();
}

QString ResourceInfo::updateTimestampField() const
{
    return d->updatedAtField;
}

QStringList ResourceInfo::fillableFields() const
{
    return d->fillableProperties;
}

bool ResourceInfo::hasHiddenFields() const
{
    return !d->hiddenFields.isEmpty();
}

QStringList ResourceInfo::hiddenFields() const
{
    return d->hiddenFields;
}

QMetaType ResourceInfo::fieldType(const QString &name) const
{
    return d->record.field(name).metaType();
}

QSqlField ResourceInfo::field(const QString &name) const
{
    return d->record.field(name);
}

QStringList ResourceInfo::fieldNames() const
{
    QStringList names;
    for (int i(0); i < d->record.count(); ++i)
        names.append(d->record.fieldName(i));
    return names;
}

QSqlRecord ResourceInfo::record() const
{
    return d->record;
}

RelationInfo ResourceInfo::relation(const QString &name) const
{
    return d->relations.value(name);
}

QStringList ResourceInfo::relationNames() const
{
    return d->relations.keys();
}

QList<RelationInfo> ResourceInfo::relations() const
{
    return d->relations.values();
}

bool ResourceInfo::loadRelations() const
{
    return d->loadRelations;
}

bool ResourceInfo::isValid() const
{
    return !d->name.isEmpty()
           && !d->table.isEmpty()
           && !d->primaryKey.isEmpty()
           && !d->record.isEmpty();
}

void ResourceInfo::load(const QString &name, const QJsonObject &object, Api *api)
{
    d->name = name;

    const QString table = object.value("table").toString();
    const QSqlRecord record = api->database().record(table);

    d->record = record;

    auto findPrimaryKey = [&table, &api](const QJsonObject &) -> QString {
        return DatabaseUtils::primaryKeyOn(table, api);
    };

    auto generateLocalKey = [&table, &api](const QJsonObject &) -> QString {
        return DatabaseUtils::foreignKeyFor(table, api);
    };

    auto generateCreationTimestamp = [&record](const QJsonObject &) {
        return (record.contains("created_at") ? "created_at" : "");
    };

    auto generateUpdateTimestamp = [&record](const QJsonObject &) {
        return (record.contains("updated_at") ? "updated_at" : "");
    };

    auto generateFillable = [this, &record](const QJsonObject &) -> QStringList {
        QStringList exceptions;
        exceptions.append(d->primaryKey); // We don't fill primary key
        exceptions.append(d->createdAtField); // we don't fill timestamps
        exceptions.append(d->updatedAtField); // we don't fill timestamps

        QStringList fields;
        for (int i(0); i < record.count(); ++i) {
            const QString fieldName = record.fieldName(i);

            // We skip exceptions
            if (exceptions.contains(fieldName))
                continue;

            fields.append(fieldName);
        }
        return fields;
    };

    auto generateHiddenFields = [&table, &api](const QJsonObject &) -> QStringList {
        return DatabaseUtils::foreignKeysOn(table, api);
    };

    beginParsing(object);

    attribute("table", d->name, &d->table);
    attribute("primary_key", Callback<QString>(findPrimaryKey), &d->primaryKey);
    attribute("local_key", Callback<QString>(generateLocalKey), &d->localKey);

    attribute("fillable", Callback<QStringList>(generateFillable), &d->fillableProperties);
    attribute("hidden", Callback<QStringList>(generateHiddenFields), &d->hiddenFields);

    beginParsing(object.value("timestamps").toObject());
    attribute("created_at", Callback<QString>(generateCreationTimestamp), &d->createdAtField);
    attribute("updated_at", Callback<QString>(generateUpdateTimestamp), &d->updatedAtField);
    endParsing();

    endParsing();

    const QJsonObject relations = object.value("relations").toObject();
    const QStringList relationNames = relations.keys();
    for (const QString &name : relationNames) {
        RelationInfo info;
        info.load(name, relations.value(name).toObject(), *this, api);
        if (true)
            d->relations.insert(name, info);
    }
}

void ResourceInfo::save(QJsonObject *object) const
{
    object->insert("table", d->table);
    object->insert("primary_key", d->primaryKey);
    object->insert("local_key", d->localKey);
    if (!d->fillableProperties.isEmpty())
        object->insert("fillable", QJsonValue::fromVariant(d->fillableProperties));
    if (!d->hiddenFields.isEmpty())
        object->insert("hidden", QJsonValue::fromVariant(d->hiddenFields));
    object->insert("load_relations", d->loadRelations);

    QJsonObject timestamps;
    if (!d->createdAtField.isEmpty())
        timestamps.insert("created_at", d->createdAtField);
    if (!d->updatedAtField.isEmpty())
        timestamps.insert("updated_at", d->updatedAtField);

    if (!timestamps.isEmpty())
        object->insert("timestamps", timestamps);

    QJsonObject relations;
    for (const RelationInfo &info : d->relations) {
        QJsonObject relation;
        info.save(&relation);
        relations.insert(info.name(), relation);
    }

    if (!relations.isEmpty())
        object->insert("relations", relations);
}

ResourceInfo ResourceInfo::pivotResourceInfo(const QString &name, const QString table, const ResourceInfo &main, const ResourceInfo &foreign, Api *api)
{
    QJsonObject object;
    object.insert("table", table);
    object.insert("primary_key", "id");

    QJsonObject relations;

    QJsonObject first;
    first.insert("table", main.table());
    first.insert("local_key", main.localKey());
    first.insert("foreign_key", main.primaryKey());
    relations.insert("first", first);

    QJsonObject second;
    second.insert("table", foreign.table());
    second.insert("local_key", foreign.localKey());
    second.insert("foreign_key", foreign.primaryKey());
    relations.insert("second", second);

    object.insert("relations", relations);

    ResourceInfo resource;
    resource.load(name, object, api);
    return resource;
}

} // namespace Sql
} // namespace RestLink
