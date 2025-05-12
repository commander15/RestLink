#ifndef RELATION_API_IMPL_H
#define RELATION_API_IMPL_H

#include "relation_base_impl.h"

namespace RestLink {
namespace Sql {

class HasOneImpl : public RestLink::Sql::SingleRelationImpl
{
public:
    HasOneImpl(Relation *relation) : SingleRelationImpl(relation) {}

    bool get() override;
    bool save() override;
    bool insert() override;
    bool update() override;
    bool deleteData() override;

    Relation::Type relationType() const override { return Relation::Type::HasOne; }
    RelationImpl *clone() const override { return new HasOneImpl(relation); }
};

class BelongsToImpl : public RestLink::Sql::SingleRelationImpl
{
public:
    BelongsToImpl(Relation *relation) : SingleRelationImpl(relation) {}

    bool get() override;
    bool save() override;
    bool insert() override;
    bool update() override;
    bool deleteData() override;

    Relation::OperationMode operationMode() const override { return Relation::OperationMode::PostProcessing; }
    Relation::Type relationType() const override { return Relation::Type::BelongsTo; }
    RelationImpl *clone() const override { return new BelongsToImpl(relation); }
};

class HasManyImpl : public RestLink::Sql::MultipleRelationImpl
{
public:
    HasManyImpl(Relation *relation) : MultipleRelationImpl(relation) {}

    bool get() override;
    bool save() override;
    bool insert() override;
    bool update() override;
    bool deleteData() override;

    Relation::Type relationType() const override { return Relation::Type::HasMany; }
    RelationImpl *clone() const override { return new HasManyImpl(relation); }
};

class BelongsToManyImpl : public RestLink::Sql::MultipleRelationImpl
{
public:
    BelongsToManyImpl(Relation *relation) : MultipleRelationImpl(relation) {}

    bool get() override;
    bool save() override;
    bool insert() override;
    bool update() override;
    bool deleteData() override;

    Relation::Type relationType() const override { return Relation::Type::BelongsToMany; }
    RelationImpl *clone() const override { return new BelongsToManyImpl(relation); }
};

} // namespace Sql
} // namespace RestLink

#endif // RELATION_API_IMPL_H
