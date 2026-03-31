#include "relation_api_impl.h"

#include <api.h>
#include <utils/queryrunner.h>

#include <QtSql/qsqlerror.h>
#include <QtSql/qsqlquery.h>

namespace RestLink {
namespace Sql {

bool HasOneImpl::get()
{
    QueryFilters filters;
    filters.andWhere(info.foreignKey(), root->primary());
    return getModel(m_relatedModel, filters);
}

bool HasOneImpl::insert()
{
    m_relatedModel.setField(info.foreignKey(), root->primary());
    return insertModel(m_relatedModel);
}

bool HasOneImpl::update()
{
    m_relatedModel.setField(info.foreignKey(), root->primary());
    return updateModel(m_relatedModel);
}

bool HasOneImpl::deleteData()
{
    return deleteModel(m_relatedModel);
}

bool BelongsToOneImpl::get()
{
    QueryFilters filters;
    filters.andWhere(info.foreignKey(), root->field(info.localKey()));
    return getModel(m_relatedModel, filters);
}

bool BelongsToOneImpl::insert()
{
    // We update foreign key on root
    root->setField(info.localKey(), m_relatedModel.primary());
    return true;
}

bool BelongsToOneImpl::update()
{
    // We update foreign key on root
    root->setField(info.localKey(), m_relatedModel.primary());
    return true;
}

bool BelongsToOneImpl::deleteData()
{
    // We update foreign key on root
    root->setField(info.localKey(), QVariant());
    return updateModel(*root);
}

bool HasManyImpl::get()
{
    QueryOptions options;
    options.filters.andWhere(info.foreignKey(), root->primary());

    bool success = false;
    m_relatedModels = Model::getMulti(foreignResource, options, root->api(), &success);
    return success && MultipleRelationImpl::get();
}

bool HasManyImpl::save()
{
    for (Model &model : m_relatedModels) {
        model.setField(info.foreignKey(), root->primary());
        if (!saveModel(model))
            return false;
    }

    // Syncing

    QueryOptions options;

    QString filter("%1 NOT IN (%2)");
    filter = filter.arg(QueryBuilder::formatFieldName(foreignResource.primaryKey(), root->api()));
    filter = filter.arg(formatedRelatedIds().join(", "));
    options.filters.andWhere(Expression(filter));

    QString deleteStatement = QueryBuilder::deleteStatement(foreignResource, options, root->api());
    bool success = false;
    QueryRunner::exec(deleteStatement, root->api(), &success);
    return success;
}

bool HasManyImpl::insert()
{
    for (Model &model : m_relatedModels) {
        model.setField(info.foreignKey(), root->primary());
        if (!insertModel(model))
            return false;
    }
    return true;
}

bool HasManyImpl::update()
{
    return save();
}

bool HasManyImpl::deleteData()
{
    for (Model &model : m_relatedModels)
        if (!deleteModel(model))
            return false;
    return true;
}

bool BelongsToManyImpl::get()
{
    Api *api = root->api();
    const QString pivot = info.pivot();

    QString statement = QStringLiteral("SELECT %1 FROM %2 WHERE %3 = %4")
                            .arg(QueryBuilder::formatFieldName(info.foreignKey(), api),
                                 QueryBuilder::formatTableName(pivot, api),
                                 QueryBuilder::formatFieldName(info.localKey(), api),
                                 QueryBuilder::formatValue(root->primary(), api));

    QSqlQuery query = exec(statement);

    QStringList ids;
    while (query.next())
        ids.append(QueryBuilder::formatValue(query.value(0), api));

    QueryOptions options;
    options.filters.andWhere(Expression(QueryBuilder::formatFieldName(foreignResource.primaryKey(), api) + " IN(" + ids.join(", ") + ')'));
    // options.withRelations = true;

    m_relatedModels = Model::getMulti(foreignResource, options, api, &query);
    if (query.lastError().type() != QSqlError::NoError)
        return false;

    return MultipleRelationImpl::get();
}

bool BelongsToManyImpl::save()
{
    for (Model &model : m_relatedModels) {
        root->setField(info.localKey(), model.primary());
        if (!saveModel(model))
            return false;
    }

    // Syncing

    QueryOptions options;

    QString filter("%1 NOT IN (%2)");
    filter = filter.arg(QueryBuilder::formatFieldName(foreignResource.primaryKey(), root->api()));
    filter = filter.arg(formatedRelatedIds().join(", "));
    options.filters.andWhere(Expression(filter));

    QString deleteStatement = QueryBuilder::deleteStatement(foreignResource, options, root->api());
    bool success = false;
    QueryRunner::exec(deleteStatement, root->api(), &success);

    return success && MultipleRelationImpl::save();
}

bool BelongsToManyImpl::insert()
{
    return save();
}

bool BelongsToManyImpl::update()
{
    return save();
}

bool BelongsToManyImpl::deleteData()
{
    return false;
}

} // namespace Sql
} // namespace RestLink
