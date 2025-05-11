#include "model.h"

#include "debug.h"
#include "modelmanager.h"
#include "querybuilder.h"
#include "jsonutils.h"

#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlerror.h>
#include <qjsonarray.h>

namespace RestLink {
namespace Sql {

Model::Model()
    : d_ptr(new ModelData)
{
}

Model::Model(const QString &table, ModelManager *manager)
    : d_ptr(new ModelData)
{
    d_ptr->table = table;
    d_ptr->manager = manager;

    manager->refModel(this);
}

Model::~Model()
{
    if (d_ptr->manager)
        d_ptr->manager->unrefModel(this);
}

QVariant Model::primary() const
{
    return field(primaryField());
}

void Model::setPrimary(const QVariant &value)
{
    setField(primaryField(), value);
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
    const QJsonArray hiddenFields = definition().value("hidden").toArray();
    for (const QJsonValue &value : hiddenFields)
        object.remove(value.toString());

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
    filters.andWhere(primaryField(), id);
    return getByFilters(filters);
}

bool Model::getByFilters(const QueryFilters &filters)
{
    QueryOptions options;
    options.filters = filters;
    options.limit = 1;

    QSqlQuery query = exec(QueryBuilder::selectStatement(definition(), options, d_ptr->manager));
    if (!query.next())
        return false;

    d_ptr->data = JsonUtils::objectFromRecord(query.record(), definition()).toVariantHash();
    return true;
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

bool Model::loadAll()
{
    const QJsonObject definition = this->definition();
    const QJsonObject relationObject = definition.value("relations").toObject();
    const QStringList names = relationObject.keys();
    return load(names);
}

bool Model::insert()
{
    QVariantHash data = d_ptr->data;
    data.remove(primaryField());

    QSqlQuery query = exec(QueryBuilder::insertStatement(definition(), data, d_ptr->manager));

    const QVariant id = query.lastInsertId();
    if (!id.isValid())
        return false;

    setPrimary(id);
    return true;
}

bool Model::update()
{
    if (!exists())
        return false;

    QueryOptions options;
    options.filters.andWhere(primaryField(), primary());

    QSqlQuery query = exec(QueryBuilder::updateStatement(definition(), d_ptr->data, options, d_ptr->manager));
    return query.lastError().type() == QSqlError::NoError;
}

bool Model::deleteData()
{
    if (!exists())
        return false;

    QueryOptions options;
    options.filters.andWhere(primaryField(), primary());

    QSqlQuery query = exec(QueryBuilder::deleteStatement(definition(), options, d_ptr->manager));
    return query.lastError().type() == QSqlError::NoError;
}

QString Model::tableName() const
{
    return definition().value("table").toString();
}

QString Model::primaryField() const
{
    return definition().value("primary").toString();
}

QJsonObject Model::definition() const
{
    return d_ptr->manager->modelDefinition(d_ptr->table);
}

QJsonObject Model::relationDefinition(const QString &name) const
{
    return definition().value("relations").toObject().value(name).toObject();
}

ModelManager *Model::manager() const
{
    return d_ptr->manager;
}

QList<Model> Model::getMulti(const QString &table, const QueryOptions &options, ModelManager *manager)
{
    const QJsonObject definition = manager->modelDefinition(table);
    const QString statement = QueryBuilder::selectStatement(definition, options, manager);

    QSqlQuery query(manager->database());
    query.setForwardOnly(true);

    if (!query.exec(statement)) {
        sqlWarning() << statement;
        sqlWarning() << query.lastError().databaseText();
        return {};
    }

    QList<Model> models;
    while (query.next()) {
        Model model(table, manager);
        model.fill(query.record());
        if (options.withRelations)
            model.loadAll();
        models.append(model);
    }
    return models;
}

int Model::count(const QString &table, const QueryOptions &options, ModelManager *manager)
{
    const QJsonObject definition = manager->modelDefinition(table);

    QString statement = QStringLiteral("SELECT COUNT(*) FROM ") + definition.value("table").toString();

    const QString whereClause = QueryBuilder::whereClause(options, manager);
    if (!whereClause.isEmpty()) {
        statement.append(' ' + whereClause);
    }

    QSqlQuery query(manager->database());
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
    if (!d_ptr->manager)
        return QSqlQuery();

    QSqlQuery query(d_ptr->manager->database());
    query.setForwardOnly(true);

#ifdef QT_DEBUG
    if (!query.exec(statement)) {
        sqlWarning() << statement;
        sqlWarning() << query.lastError().databaseText();
    }
#endif

    return query;
}

} // namespace Sql
} // namespace RestLink
