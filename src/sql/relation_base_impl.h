#ifndef RELATION_BASE_IMPL_H
#define RELATION_BASE_IMPL_H

#include "relation.h"
#include "model.h"

namespace RestLink {
namespace Sql {

class SingleRelationImpl : public RelationImpl
{
public:
    SingleRelationImpl(Relation *relation);
    virtual ~SingleRelationImpl() = default;

    QVariant field(const QString &name) const override;
    void setField(const QString &name, const QVariant &value) override;

    QList<Model> relatedModels() const override;
    void setRelatedModels(const QList<Model> &models) override;

    QJsonValue jsonValue() const override;
    void setJsonValue(const QJsonValue &value) override;

    bool exists() const override;
    bool get() override;
    bool save() override;
    bool insert() override;
    bool update() override;
    bool deleteData() override;

protected:
    Model m_relatedModel;
};

class MultipleRelationImpl : public RelationImpl
{
public:
    MultipleRelationImpl(Relation *relation);
    virtual ~MultipleRelationImpl() = default;

    QVariant field(const QString &name) const override;
    void setField(const QString &name, const QVariant &value) override;

    QList<Model> relatedModels() const override;
    void setRelatedModels(const QList<Model> &models) override;

    QJsonValue jsonValue() const override;
    void setJsonValue(const QJsonValue &value) override;

    bool exists() const override;
    bool get() override;
    bool save() override;
    bool insert() override;
    bool update() override;
    bool deleteData() override;

private:
    QList<Model> m_relatedModels;
};

} // namespace Sql
} // namespace RestLink

#endif // RELATION_BASE_IMPL_H
