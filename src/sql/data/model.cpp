#include "model.h"

#include <debug.h>
#include <api.h>

#include <meta/resourceinfo.h>
#include <meta/relationinfo.h>

#include <utils/querybuilder.h>
#include <utils/jsonutils.h>

#include <QtCore/qjsonarray.h>

#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlerror.h>

namespace RestLink {
namespace Sql {

class ModelData final : public QSharedData
{
public:
    ModelData() = default;

    ModelData(const ModelData &other)
        : QSharedData(other),
        data(other.data),
        relations(other.relations),
        resource(other.resource),
        api(other.api)
    {}

    ModelData(ModelData &&other) = default;

    QVariantHash data;
    QList<Relation> relations;

    QSqlQuery lastQuery;

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

void Model::fill(const QJsonObject &data)
{
    d_ptr->data = data.toVariantHash();

    for (Relation &relation : d_ptr->relations)
        relation.fill(data);
}

void Model::fill(const QSqlRecord &record)
{
    d_ptr->data.clear();

    for (int i(0); i < record.count(); ++i)
        d_ptr->data.insert(record.fieldName(i), record.value(i));

    for (Relation &relation : d_ptr->relations)
        relation.fill(record);
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
        d_ptr->relations.append(relation);
    }

    return true;
}

bool Model::insert()
{
    QVariantHash data = d_ptr->data;
    data.remove(d_ptr->resource.primaryKey());

    QSqlQuery query = exec(QueryBuilder::insertStatement(d_ptr->resource, data, d_ptr->api));

    const QVariant id = query.lastInsertId();
    if (!id.isValid())
        return false;

    setPrimary(id);

    for (Relation &relation : d_ptr->relations)
        if (!relation.insert())
            return false;

    return true;
}

bool Model::update()
{
    if (!exists())
        return false;

    QueryOptions options;
    options.filters.andWhere(d_ptr->resource.primaryKey(), primary());

    QSqlQuery query = exec(QueryBuilder::updateStatement(d_ptr->resource, d_ptr->data, options, d_ptr->api));
    if (query.lastError().type() != QSqlError::NoError)
        return false;

    for (Relation &relation : d_ptr->relations)
        if (!relation.update())
            return false;

    return true;
}

bool Model::deleteData()
{
    if (!exists())
        return false;

    QueryOptions options;
    options.filters.andWhere(d_ptr->resource.primaryKey(), primary());

    QSqlQuery query = exec(QueryBuilder::deleteStatement(d_ptr->resource, options, d_ptr->api));
    if (query.lastError().type() != QSqlError::NoError)
        return false;

    for (Relation &relation : d_ptr->relations)
        if (!relation.deleteData())
            return false;

    return true;
}

const QSqlQuery &Model::lastQuery() const
{
    return d_ptr->lastQuery;
}

RelationInfo Model::relationInfo(const QString &name) const
{
    return d_ptr->resource.relation(name);
}

ResourceInfo Model::resourceInfo() const
{
    return d_ptr->api->resourceInfo(d_ptr->resource.table());
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
    if (!d_ptr->api)
        return QSqlQuery();

    QSqlQuery query(d_ptr->api->database());
    query.setForwardOnly(true);

#ifdef QT_DEBUG
    sqlInfo() << statement;
    if (!query.exec(statement)) {
        sqlWarning() << statement;
        sqlWarning() << query.lastError().databaseText();
    }
#endif

    return query;
}

} // namespace Sql
} // namespace RestLink
