#ifndef CRUDINTERFACE_H
#define CRUDINTERFACE_H

#include <global.h>

namespace RestLink {
namespace Sql {

class SQL_EXPORT CRUDInterface
{
public:
    virtual ~CRUDInterface() = default;

    virtual bool exists() const = 0;

    virtual bool get() = 0;

    virtual bool save();
    virtual bool insert() = 0;
    virtual bool update() = 0;

    virtual bool deleteData() = 0;
};

} // namespace Sql
} // namespace RestLink

#endif // CRUDINTERFACE_H
