#include "relation_api_impl.h"
#include "debug.h"
#include "modelmanager.h"
#include <qsqlerror.h>
#include <qsqlquery.h>

namespace RestLink {
namespace Sql {

bool HasOneImpl::get()
{
    QueryFilters filters;
    filters.andWhere(rootForeignField(), rootPrimaryValue());
    return m_relatedModel.getByFilters(filters);
}

bool HasOneImpl::save()
{
    return false;
}

bool HasOneImpl::insert()
{
    return false;
}

bool HasOneImpl::update()
{
    return false;
}

bool HasOneImpl::deleteData()
{
    return false;
}

bool BelongsToImpl::get()
{
    QueryFilters filters;
    filters.andWhere(relatedPrimaryField(), rootValue(relatedForeignField()));
    return m_relatedModel.getByFilters(filters);
}

bool BelongsToImpl::save()
{
    if (!m_relatedModel.save())
        return false;

    setRootValue(relatedForeignField(), m_relatedModel.primary());
    return true;
}

bool BelongsToImpl::insert()
{
    if (!m_relatedModel.insert())
        return false;

    setRootValue(relatedForeignField(), m_relatedModel.primary());
    return true;
}

bool BelongsToImpl::update()
{
    return false;
}

bool BelongsToImpl::deleteData()
{
    return false;
}

bool HasManyImpl::get()
{
    QueryOptions options;
    options.filters.andWhere(rootForeignField(), rootPrimaryValue());

    // ToDo: must handle error in case no related found
    m_relatedModels = Model::getMulti(relation->modelName(), options, rootModel()->manager());
    return !m_relatedModels.isEmpty();
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
    ModelManager *manager = rootModel()->manager();
    const QString pivot = relation->relationDefinition().value("pivot").toString();

    QString statement = "SELECT " + relatedForeignField() + " FROM " + pivot;
    statement.append(" WHERE " + rootPrimaryField() + " = " + QueryBuilder::formatValue(rootPrimaryValue(), manager));

    QSqlQuery query(relation->root()->manager()->database());
    query.setForwardOnly(true);
    if (!query.exec(statement)) {
        sqlDebug() << query.lastError().databaseText();
        return false;
    }

    QStringList ids;
    while (query.next())
        ids.append(QueryBuilder::formatValue(query.value(0), manager));

    QueryOptions options;
    options.filters.andWhere(Expression("id IN(" + ids.join(", ") + ')'));
    // options.withRelations = true;

    // ToDo: must handle error in case no related found
    m_relatedModels = Model::getMulti(relation->modelName(), options, manager);
    return !m_relatedModels.isEmpty();
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
