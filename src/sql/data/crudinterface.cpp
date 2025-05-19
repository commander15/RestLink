#include "crudinterface.h"

namespace RestLink {
namespace Sql {

bool CRUDInterface::save()
{
    if (exists())
        return update();
    else
        return insert();
}

} // namespace Sql
} // namespace RestLink
