#include "relation_api_impl.h"

namespace RestLink {
namespace Sql {

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

} // namespace Sql
} // namespace RestLink
