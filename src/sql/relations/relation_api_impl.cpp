#include "relation_api_impl.h"

#include <api.h>

#include <QtSql/qsqlerror.h>
#include <QtSql/qsqlquery.h>

namespace RestLink {
namespace Sql {

bool HasOneImpl::get()
{
    QueryFilters filters;
    filters.andWhere(info.foreignKey(), root->primary());

    m_relatedModel = createModel();

    if (!m_relatedModel.getByFilters(filters))
        return false;

    return true;
}

bool HasOneImpl::insert()
{
    // If root doesn't own the resource, it can't create it
    if (!info.owned())
        return true;

    m_relatedModel.setField(info.foreignKey(), root->primary());
    return m_relatedModel.insert();
}

bool HasOneImpl::update()
{
    // If root doesn't own the resource, it can't modiffy it
    if (!info.owned())
        return true;

    m_relatedModel.setField(info.foreignKey(), root->primary());
    return m_relatedModel.update();
}

bool HasOneImpl::deleteData()
{
    // If root doesn't own the resource, it can't delete it
    if (!info.owned())
        return true;

    return m_relatedModel.deleteData();
}

bool BelongsToOneImpl::get()
{
    QueryFilters filters;
    filters.andWhere(foreignResource.primaryKey(), root->field(info.foreignKey()));

    m_relatedModel = createModel();

    if (!m_relatedModel.getByFilters(filters))
        return false;

    return true;
}

bool BelongsToOneImpl::insert()
{
    if (info.owned() && !m_relatedModel.insert())
        return false;

    // We update foreign key on root
    root->setField(info.foreignKey(), m_relatedModel.primary());

    return true;
}

bool BelongsToOneImpl::update()
{
    if (info.owned() && !m_relatedModel.update())
        return false;

    // We update foreign key on root
    root->setField(info.foreignKey(), m_relatedModel.primary());

    return true;
}

bool BelongsToOneImpl::deleteData()
{
    if (info.owned() && !m_relatedModel.deleteData())
        return false;

    root->setField(info.foreignKey(), QVariant());

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

bool BelongsToManyThroughImpl::get()
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

    m_relatedModels = Model::getMulti(foreignResource, options, api, &query);
    return query.lastError().type() == QSqlError::NoError;
}

bool BelongsToManyThroughImpl::save()
{
    return false;
}

bool BelongsToManyThroughImpl::insert()
{
    return false;
}

bool BelongsToManyThroughImpl::update()
{
    return false;
}

bool BelongsToManyThroughImpl::deleteData()
{
    return false;
}

} // namespace Sql
} // namespace RestLink
