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
    : d_ptr(new ModelData())
{
}

Model::Model(const QString &resource, Api *api)
    : Model(api->resourceInfo(resource), api)
{
}

Model::Model(const ResourceInfo &resource, Api *api)
    : d_ptr(new ModelData())
{
    d_ptr->resource = resource;
    d_ptr->api = api;

    api->refModel(this);
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
    const QString field = d_ptr->resource.updateTimestampField();
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
    const ResourceInfo resource = d_ptr->resource;

    QStringList fieldNames = resource.fillableFields();
    switch (mode) {
    case FullFill:
        fieldNames.prepend(resource.primaryKey());

    case ExtendedFill:
        if (resource.hasHiddenFields())
            fieldNames.append(resource.hiddenFields());
        if (resource.hasCreationTimestamp())
            fieldNames.append(resource.creationTimestampField());
        if (resource.hasUpdateTimestamp())
            fieldNames.append(resource.updateTimestampField());

    default:
        break;
    }

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
    for (const Relation &relation : std::as_const(d_ptr->relations))
        object.insert(relation.relationName(), relation.jsonValue());

    return object;
}

bool Model::exists() const
{
    return primary().isValid();
}

bool Model::get()
{
    const QString primaryKey = d_ptr->resource.primaryKey();

    QueryFilters filters;
    filters.andWhere(primaryKey, field(primaryKey));
    return getByFilters(filters);
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
    return loadDefault();
}

bool Model::loadAll()
{
    return load(d_ptr->resource.relationNames());
}

bool Model::loadDefault()
{
    QStringList names = d_ptr->resource.relationNames();
    for (const QString &name : std::as_const(names))
        if (!d_ptr->resource.relation(name).autoLoadable())
            names.removeOne(name);
    return load(names);
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
        relation.prepareOperations(this, Relation::PreProcessing);
        if (!relation.insert())
            return false;
    }

    QSqlQuery query = exec(QueryBuilder::insertStatement(d_ptr->resource, d_ptr->data, d_ptr->api));

    const QVariant id = query.lastInsertId();
    if (!id.isValid())
        return false;

    setPrimary(id);

    for (Relation &relation : d_ptr->relations) {
        relation.prepareOperations(this, Relation::PostProcessing);
        if (!relation.insert())
            return false;
    }

    return true;
}

bool Model::update()
{
    if (!exists())
        return false;

    const QString timestampField = d_ptr->resource.updateTimestampField();
    if (!timestampField.isEmpty())
        d_ptr->data.insert(timestampField, QDateTime::currentDateTime());

    for (Relation &relation : d_ptr->relations) {
        relation.prepareOperations(this, Relation::PreProcessing);
        if (!relation.update())
            return false;
    }

    QueryOptions options;
    options.filters.andWhere(d_ptr->resource.primaryKey(), primary());

    QSqlQuery query = exec(QueryBuilder::updateStatement(d_ptr->resource, d_ptr->data, options, d_ptr->api));
    if (query.lastError().type() != QSqlError::NoError)
        return false;

    for (Relation &relation : d_ptr->relations) {
        relation.prepareOperations(this, Relation::PostProcessing);
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
        relation.prepareOperations(this, Relation::PreProcessing);
        if (!relation.deleteData())
            return false;
    }

    QueryOptions options;
    options.filters.andWhere(d_ptr->resource.primaryKey(), primary());

    QSqlQuery query = exec(QueryBuilder::deleteStatement(d_ptr->resource, options, d_ptr->api));
    if (query.lastError().type() != QSqlError::NoError)
        return false;

    for (Relation &relation : d_ptr->relations) {
        relation.prepareOperations(this, Relation::PostProcessing);
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

QList<Model> Model::getMulti(const QString &resource, const QueryOptions &options, Api *api, bool *success)
{
    QSqlQuery query(api->database());

    const QList<Model> models = getMulti(api->resourceInfo(resource), options, api, &query);
    *success = query.lastError().type() == QSqlError::NoError;
    return models;
}

QList<Model> Model::getMulti(const ResourceInfo &resource, const QueryOptions &options, Api *api, bool *success)
{
    QSqlQuery query(api->database());

    const QList<Model> models = getMulti(resource, options, api, &query);
    *success = query.lastError().type() == QSqlError::NoError;
    return models;
}

QList<Model> Model::getMulti(const QString &resource, const QueryOptions &options, Api *api, QSqlError *error)
{
    QSqlQuery query(api->database());

    const QList<Model> models = getMulti(api->resourceInfo(resource), options, api, &query);
    *error = query.lastError();
    return models;
}

QList<Model> Model::getMulti(const ResourceInfo &resource, const QueryOptions &options, Api *api, QSqlError *error)
{
    QSqlQuery query(api->database());

    const QList<Model> models = getMulti(resource, options, api, &query);
    *error = query.lastError();
    return models;
}

QList<Model> Model::getMulti(const QString &resource, const QueryOptions &options, Api *api, QSqlQuery *query)
{
    return getMulti(api->resourceInfo(resource), options, api, query);
}

QList<Model> Model::getMulti(const ResourceInfo &resource, const QueryOptions &options, Api *api, QSqlQuery *query)
{
    const QString statement = QueryBuilder::selectStatement(resource, options, api);

    bool success = false;
    QSqlQuery sqlQuery = QueryRunner::exec(statement, api, &success);
    if (!success) {
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
    return count(api->resourceInfo(resource), options, api);
}

int Model::count(const ResourceInfo &resource, const QueryOptions &options, Api *api)
{
    QString statement = QStringLiteral("SELECT COUNT(*) FROM ") + resource.table();

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
