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
    bool insert() override;
    bool update() override;
    bool deleteData() override;

    Relation::OperationMode operationMode() const override { return Relation::PostProcessing; }
    Relation::Type relationType() const override { return Relation::Type::HasOne; }
    RelationImpl *clone() const override { return new HasOneImpl(relation); }
};

class BelongsToOneImpl : public RestLink::Sql::SingleRelationImpl
{
public:
    BelongsToOneImpl(Relation *relation) : SingleRelationImpl(relation) {}

    bool get() override;
    bool insert() override;
    bool update() override;
    bool deleteData() override;

    //void setJsonValue(const QJsonValue &value) override;

    Relation::OperationMode operationMode() const override { return Relation::OperationMode::PreProcessing; }
    Relation::Type relationType() const override { return Relation::Type::BelongsToOne; }
    RelationImpl *clone() const override { return new BelongsToOneImpl(relation); }
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

    Relation::OperationMode operationMode() const override { return Relation::OperationMode::PreProcessing; }
    Relation::Type relationType() const override { return Relation::Type::HasMany; }
    RelationImpl *clone() const override { return new HasManyImpl(relation); }
};

class BelongsToManyThroughImpl : public RestLink::Sql::MultipleRelationImpl
{
public:
    BelongsToManyThroughImpl(Relation *relation) : MultipleRelationImpl(relation) {}

    bool get() override;
    bool save() override;
    bool insert() override;
    bool update() override;
    bool deleteData() override;

    Relation::OperationMode operationMode() const override { return Relation::OperationMode::PostProcessing; }
    Relation::Type relationType() const override { return Relation::Type::BelongsToManyThrough; }
    RelationImpl *clone() const override { return new BelongsToManyThroughImpl(relation); }
};

} // namespace Sql
} // namespace RestLink

#endif // RELATION_API_IMPL_H
