#ifndef RELATION_BASE_IMPL_H
#define RELATION_BASE_IMPL_H

#include <data/relation.h>
#include <data/model.h>

namespace RestLink {
namespace Sql {

class SingleRelationImpl : public RelationImpl
{
public:
    SingleRelationImpl(Relation *relation);
    virtual ~SingleRelationImpl() = default;

    QList<Model> relatedModels() const override;
    void setRelatedModels(const QList<Model> &models) override;

    QJsonValue jsonValue() const override;
    void setJsonValue(const QJsonValue &value) override;

    bool exists() const override;

    bool get() override;

protected:
    Model m_relatedModel;
};

class MultipleRelationImpl : public RelationImpl
{
public:
    MultipleRelationImpl(Relation *relation);
    virtual ~MultipleRelationImpl() = default;

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
    QStringList formatedRelatedIds() const;

    QList<Model> m_relatedModels;
};

class MultipleThroughRelationImpl : public MultipleRelationImpl
{
public:
    MultipleThroughRelationImpl(Relation *relation);
    virtual ~MultipleThroughRelationImpl() = default;

    QVariant field(const QString &name, int index) const override;
    void setField(const QString &name, const QVariant &value, int index) override;

    QJsonValue jsonValue() const override;
    void setJsonValue(const QJsonValue &value) override;

protected:
    QList<QVariantHash> m_pivots;
};

} // namespace Sql
} // namespace RestLink

#endif // RELATION_BASE_IMPL_H
