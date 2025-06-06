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

bool CRUDInterface::exec(Operation op)
{
    switch (op) {
    case GetOperation:
        return get();

    case SaveOperation:
        return (exists() ? update() : insert());

    case InsertOperation:
        return insert();

    case UpdateOperation:
        return update();

    case DeleteOperation:
        return deleteData();

    default:
        return false;
    }
}

} // namespace Sql
} // namespace RestLink
