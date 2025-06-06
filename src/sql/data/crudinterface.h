#ifndef CRUDINTERFACE_H
#define CRUDINTERFACE_H

#include <global.h>

namespace RestLink {
namespace Sql {

class SQL_EXPORT CRUDInterface
{
public:
    enum Operation {
        GetOperation,
        SaveOperation,
        InsertOperation,
        UpdateOperation,
        DeleteOperation
    };

    virtual ~CRUDInterface() = default;

    virtual bool exists() const = 0;

    virtual bool get() = 0;

    virtual bool save();
    virtual bool insert() = 0;
    virtual bool update() = 0;

    virtual bool deleteData() = 0;

    bool exec(Operation op);
};

} // namespace Sql
} // namespace RestLink

#endif // CRUDINTERFACE_H
