#include "model.h"

#include <debug.h>
#include <api.h>

#include <meta/resourceinfo.h>
#include <meta/relationinfo.h>

#include <utils/queryrunner.h>
#include <utils/querybuilder.h>
#include <utils/jsonutils.h>

#include <QtCore/qjsonarray.h>

#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlerror.h>

namespace RestLink {
namespace Sql {

class ModelData : public QSharedData
{
public:
    QVariantHash data;
    QHash<QString, Relation> relations;

    QJsonObject lastQuery;
    QJsonObject lastError;

    ResourceInfo resource;
    Api *api = nullptr;
};

Model::Model()
    : d_ptr(new ModelData)
{
}

Model::Model(const QString &resource, Api *manager)
    : d_ptr(new ModelData)
{
    d_ptr->resource = manager->resourceInfo(resource);
    d_ptr->api = manager;

    manager->refModel(this);
}

Model::Model(const Model &other) = default;

Model::~Model()
{
    if (d_ptr->api)
        d_ptr->api->unrefModel(this);
}

Model &Model::operator=(const Model &other) = default;

QVariant Model::primary() const
{
    return field(d_ptr->resource.primaryKey());
}

void Model::setPrimary(const QVariant &value)
{
    setField(d_ptr->resource.primaryKey(), value);
}

QDateTime Model::createdAt() const
{
    const QString field = d_ptr->resource.creationTimestampField();
    return this->field(field).toDateTime();
}

QDateTime Model::updatedAt() const
{
    const QString field = d_ptr->resource.updateTimestamp();
    return this->field(field).toDateTime();
}

QVariant Model::field(const QString &name) const
{
    return d_ptr->data.value(name);
}

void Model::setField(const QString &name, const QVariant &value)
{
    if (!value.isNull())
        d_ptr->data.insert(name, value);
    else if (d_ptr->data.contains(name))
        d_ptr->data.remove(name);
}

Relation Model::relation(const QString &name) const
{
    return d_ptr->relations.value(name);
}

void Model::fill(const QJsonObject &data, FillMode mode)
{
    QStringList fieldNames = d_ptr->resource.fillableFields();
    if (mode == FullFill)
        fieldNames.prepend(d_ptr->resource.primaryKey());

    const QStringList relationNames = d_ptr->resource.relationNames();
    const QStringList inputs = data.keys();

    for (const QString &input : inputs) {
        const QJsonValue value = data.value(input);

        if (fieldNames.contains(input)) {
            d_ptr->data.insert(input, value.toVariant());
        } else if (d_ptr->relations.contains(input)) {
            d_ptr->relations[input].fill(value);
        } else if (relationNames.contains(input)) {
            Relation relation(input, this);
            relation.fill(value);
            d_ptr->relations.insert(input, relation);
        }
    }

    return;
    for (Relation &relation : d_ptr->relations) {
        const QJsonValue value = data.value(relation.relationName());
        relation.fill(value);
    }
}

void Model::fill(const QSqlRecord &record)
{
    d_ptr->data.clear();

    for (int i(0); i < record.count(); ++i)
        d_ptr->data.insert(record.fieldName(i), record.value(i));
}

QJsonObject Model::jsonObject() const
{
    QJsonObject object = QJsonObject::fromVariantHash(d_ptr->data);

    // First, we remove fields marked as hidden
    const QStringList hiddenFields = d_ptr->resource.hiddenFields();
    for (const QString &field : hiddenFields)
        object.remove(field);

    // Next, we add relations
    for (const Relation &relation : d_ptr->relations)
        object.insert(relation.relationName(), relation.jsonValue());

    return object;
}

bool Model::exists() const
{
    return primary().isValid();
}

bool Model::get()
{
    return get(primary());
}

bool Model::get(const QVariant &id)
{
    QueryFilters filters;
    filters.andWhere(d_ptr->resource.primaryKey(), id);
    return getByFilters(filters);
}

bool Model::getByFilters(const QueryFilters &filters)
{
    QueryOptions options;
    options.filters = filters;
    options.limit = 1;

    QSqlQuery query = exec(QueryBuilder::selectStatement(d_ptr->resource, options, d_ptr->api));
    if (!query.next())
        return false;

    d_ptr->data = JsonUtils::objectFromRecord(query.record(), d_ptr->resource).toVariantHash();
    return true;
}

bool Model::loadAll()
{
    return load(d_ptr->resource.relationNames());
}

bool Model::load(const QStringList &relations)
{
    d_ptr->relations.clear();
    for (const QString &name : relations) {
        Relation relation(name, this);
        if (!relation.get())
            return false;
        d_ptr->relations.insert(name, relation);
    }

    return true;
}

bool Model::insert()
{
    const QString timestampField = d_ptr->resource.creationTimestampField();
    if (!timestampField.isEmpty())
        d_ptr->data.insert(timestampField, QDateTime::currentDateTime());

    for (Relation &relation : d_ptr->relations) {
        relation.prepareOperations(Relation::PreProcessing);
        if (!relation.insert())
            return false;
    }

    QSqlQuery query = exec(QueryBuilder::insertStatement(d_ptr->resource, d_ptr->data, d_ptr->api));

    const QVariant id = query.lastInsertId();
    if (!id.isValid())
        return false;

    setPrimary(id);

    for (Relation &relation : d_ptr->relations) {
        relation.prepareOperations(Relation::PostProcessing);
        if (!relation.insert())
            return false;
    }

    return true;
}

bool Model::update()
{
    if (!exists())
        return false;

    const QString timestampField = d_ptr->resource.updateTimestamp();
    if (!timestampField.isEmpty())
        d_ptr->data.insert(timestampField, QDateTime::currentDateTime());

    for (Relation &relation : d_ptr->relations) {
        relation.prepareOperations(Relation::PreProcessing);
        if (!relation.insert())
            return false;
    }

    QueryOptions options;
    options.filters.andWhere(d_ptr->resource.primaryKey(), primary());

    QSqlQuery query = exec(QueryBuilder::updateStatement(d_ptr->resource, d_ptr->data, options, d_ptr->api));
    if (query.lastError().type() != QSqlError::NoError)
        return false;

    for (Relation &relation : d_ptr->relations)
        if (!relation.update())
            return false;

    for (Relation &relation : d_ptr->relations) {
        relation.prepareOperations(Relation::PostProcessing);
        if (!relation.update())
            return false;
    }

    return true;
}

bool Model::deleteData()
{
    if (!exists())
        return false;

    for (Relation &relation : d_ptr->relations) {
        relation.prepareOperations(Relation::PreProcessing);
        if (!relation.deleteData())
            return false;
    }

    QueryOptions options;
    options.filters.andWhere(d_ptr->resource.primaryKey(), primary());

    QSqlQuery query = exec(QueryBuilder::deleteStatement(d_ptr->resource, options, d_ptr->api));
    if (query.lastError().type() != QSqlError::NoError)
        return false;

    for (Relation &relation : d_ptr->relations)
        if (!relation.deleteData())
            return false;

    for (Relation &relation : d_ptr->relations) {
        relation.prepareOperations(Relation::PostProcessing);
        if (!relation.deleteData())
            return false;
    }

    return true;
}

QJsonObject Model::lastQuery() const
{
    return d_ptr->lastQuery;
}

QJsonObject Model::lastError() const
{
    return d_ptr->lastError;
}

RelationInfo Model::relationInfo(const QString &name) const
{
    return d_ptr->resource.relation(name);
}

ResourceInfo Model::resourceInfo() const
{
    return d_ptr->resource;
}

bool Model::isValid() const
{
    return d_ptr->resource.isValid();
}

Api *Model::api() const
{
    return d_ptr->api;
}

QList<Model> Model::getMulti(const QString &resource, const QueryOptions &options, Api *api, QSqlQuery *query)
{
    const ResourceInfo info = api->resourceInfo(resource);
    const QString statement = QueryBuilder::selectStatement(info, options, api);

    QSqlQuery sqlQuery(api->database());
    sqlQuery.setForwardOnly(true);

    if (!sqlQuery.exec(statement)) {
        sqlWarning() << statement;
        sqlWarning() << sqlQuery.lastError().databaseText();
        if (query) query->swap(sqlQuery);
        return {};
    }

    QList<Model> models;
    while (sqlQuery.next()) {
        Model model(resource, api);
        model.fill(sqlQuery.record());
        if (options.withRelations)
            model.loadAll();
        models.append(model);
    }
    return models;
}

int Model::count(const QString &resource, const QueryOptions &options, Api *api)
{
    const ResourceInfo info = api->resourceInfo(resource);
    QString statement = QStringLiteral("SELECT COUNT(*) FROM ") + info.table();

    const QString whereClause = QueryBuilder::whereClause(options, api);
    if (!whereClause.isEmpty()) {
        statement.append(' ' + whereClause);
    }

    QSqlQuery query(api->database());
    query.setForwardOnly(true);

    if (!query.exec(statement)) {
        sqlWarning() << statement;
        sqlWarning() << query.lastError().databaseText();
        return 0;
    }

    if (!query.next())
        return 0;

    return query.value(0).toInt();
}

QSqlQuery Model::exec(const QString &statement)
{
    QSqlQuery query = QueryRunner::exec(statement, d_ptr->api);

    d_ptr->lastQuery = JsonUtils::objectFromQuery(query);
    if (d_ptr->lastQuery.contains("error"))
        d_ptr->lastError = d_ptr->lastQuery.value("error").toObject();

    return query;
}

} // namespace Sql
} // namespace RestLink
