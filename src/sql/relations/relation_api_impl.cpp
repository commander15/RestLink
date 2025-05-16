#include "relation_api_impl.h"

#include <api.h>

#include <QtSql/qsqlerror.h>
#include <QtSql/qsqlquery.h>

namespace RestLink {
namespace Sql {

bool HasOneImpl::get()
{
    QueryFilters filters;
    filters.andWhere(rootResource.foreignKey(), root->primary());

    m_relatedModel = createModel();
    return m_relatedModel.getByFilters(filters);
}

bool HasOneImpl::insert()
{
    // If root doesn't own the resource, it can't create it
    if (!info.owned())
        return true;

    m_relatedModel.setField(rootResource.foreignKey(), root->primary());
    return m_relatedModel.insert();
}

bool HasOneImpl::update()
{
    // If root doesn't own the resource, it can't modiffy it
    if (!info.owned())
        return true;

    m_relatedModel.setField(rootResource.foreignKey(), root->primary());
    return m_relatedModel.update();
}

bool HasOneImpl::deleteData()
{
    // If root doesn't own the resource, it can't delete it
    if (!info.owned())
        return true;

    return m_relatedModel.deleteData();
}

bool BelongsToImpl::get()
{
    const QString foreignKey = foreignResource.foreignKey();

    QueryFilters filters;
    filters.andWhere(foreignResource.primaryKey(), root->field(foreignKey));

    m_relatedModel = createModel();
    return m_relatedModel.getByFilters(filters);
}

bool BelongsToImpl::insert()
{
    // If root doesn't own the resource, it can't create it
    if (!info.owned())
        return true;

    if (!m_relatedModel.insert())
        return false;

    // We update foreign key on root
    root->setField(foreignResource.foreignKey(), m_relatedModel.primary());
    return true;
}

bool BelongsToImpl::update()
{
    // If root doesn't own the resource, it can't modiffy it
    if (!info.owned())
        return true;

    if (!m_relatedModel.update())
        return false;

    // We update foreign key on root
    root->setField(foreignResource.foreignKey(), m_relatedModel.primary());
    return true;
}

bool BelongsToImpl::deleteData()
{
    // If root doesn't own the resource, it can't delete it
    if (!info.owned())
        return true;

    if (!m_relatedModel.deleteData())
        return false;

    return true;
}

bool HasManyImpl::get()
{
    QueryOptions options;
    options.filters.andWhere(rootResource.foreignKey(), root->primary());

    QSqlQuery query;
    m_relatedModels = Model::getMulti(relation->modelName(), options, root->api(), &query);
    return query.lastError().type() == QSqlError::NoError;
}

bool HasManyImpl::save()
{
    return false;
}

bool HasManyImpl::insert()
{
    return false;
}

bool HasManyImpl::update()
{
    return false;
}

bool HasManyImpl::deleteData()
{
    return false;
}

bool BelongsToManyImpl::get()
{
    Api *api = root->api();
    const QString pivot = info.pivot();

    QString statement = "SELECT " + foreignResource.foreignKey() + " FROM " + pivot;
    statement.append(" WHERE " + rootResource.primaryKey() + " = " + QueryBuilder::formatValue(root->primary(), api));

    QSqlQuery query = exec(statement);

    QStringList ids;
    while (query.next())
        ids.append(QueryBuilder::formatValue(query.value(0), api));

    QueryOptions options;
    options.filters.andWhere(Expression("id IN(" + ids.join(", ") + ')'));
    // options.withRelations = true;

    m_relatedModels = Model::getMulti(relation->modelName(), options, api, &query);
    return query.lastError().type() == QSqlError::NoError;
}

bool BelongsToManyImpl::save()
{
    return false;
}

bool BelongsToManyImpl::insert()
{
    return false;
}

bool BelongsToManyImpl::update()
{
    return false;
}

bool BelongsToManyImpl::deleteData()
{
    return false;
}

} // namespace Sql
} // namespace RestLink
