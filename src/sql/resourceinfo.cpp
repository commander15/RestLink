#include "resourceinfo.h"

#include "api.h"
#include "relationinfo.h"
#include "querybuilder.h"

#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonvalue.h>

#include <QtSql/qsqlrecord.h>
#include <qsqlfield.h>
#include <qsqlindex.h>

namespace RestLink {
namespace Sql {

class ResourceInfoData : public QSharedData
{
public:
    QString name;
    QString table;
    QString primaryKey;
    QString foreignKey;
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

QString ResourceInfo::foreignKey() const
{
    return d->foreignKey;
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
           && !d->record.isEmpty();
}

void ResourceInfo::load(const QString &name, const QJsonObject &object, Api *api)
{
    beginParsing(object);

    d->name = name;

    auto findPrimaryKey = [this, &api](const QJsonObject &) -> QString {
        const QSqlIndex index = api->database().primaryIndex(d->table);
        if (index.count() == 1)
            return index.fieldName(0);
        return "id";
    };

    auto generateForeignKey = [this](const QJsonObject &) -> QString {
        QString name = d->table;
        if (name.endsWith('s'))
            name.removeLast();
        return name.toLower() + '_' + d->primaryKey;
    };

    auto generateCreationTimestamp = [this](const QJsonObject &) {
        const QStringList fields = fieldNames();
        return (fields.contains("created_at") ? "created_at" : "");
    };

    auto generateUpdateTimestamp = [this](const QJsonObject &) {
        const QStringList fields = fieldNames();
        return (fields.contains("updated_at") ? "updated_at" : "");
    };

    auto generateFillable = [this](const QJsonObject &) -> QStringList {
        QStringList exceptions;
        exceptions.append(d->primaryKey); // We don't fill primary key
        exceptions.append(d->createdAtField); // we don't fill timestamps
        exceptions.append(d->updatedAtField); // we don't fill timestamps

        QStringList fields;
        for (int i(0); i < d->record.count(); ++i) {
            const QString fieldName = d->record.fieldName(i);

            // We skip exceptions
            if (exceptions.contains(fieldName))
                continue;

            fields.append(fieldName);
        }
        return fields;
    };

    auto generateHiddenFields = [this](const QJsonObject &) -> QStringList {
        QStringList fields;
        for (const RelationInfo &relation : std::as_const(d->relations)) {
            const QString foreignKey = relation.foreignKey();
            if (d->record.contains(foreignKey))
                fields.append(foreignKey);
        }
        return fields;
    };

    attribute("table", d->name, &d->table);
    attribute("primary_key", Callback<QString>(findPrimaryKey), &d->primaryKey);
    attribute("foreign_key", Callback<QString>(generateForeignKey), &d->foreignKey);
    d->record = api->database().record(d->table);

    const QJsonObject timestamps = object.value("timestamps").toObject();
    beginParsing(timestamps);
    attribute("created_at", Callback<QString>(generateCreationTimestamp), &d->createdAtField);
    attribute("updated_at", Callback<QString>(generateUpdateTimestamp), &d->updatedAtField);
    endParsing();

    attribute("fillable", Callback<QStringList>(generateFillable), &d->fillableProperties);
    attribute("hidden", Callback<QStringList>(generateHiddenFields), &d->hiddenFields);
    attribute("load_relations", false, &d->loadRelations);

    const QJsonObject relations = object.value("relations").toObject();
    const QStringList relationNames = relations.keys();
    for (const QString &name : relationNames) {
        RelationInfo info;
        info.load(name, relations.value(name).toObject(), *this, api);
        if (true)
            d->relations.insert(name, info);
    }

    endParsing();
}

void ResourceInfo::save(QJsonObject *object) const
{
    object->insert("table", d->table);
    object->insert("primary_key", d->primaryKey);
    object->insert("foreign_key", d->foreignKey);
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

} // namespace Sql
} // namespace RestLink
