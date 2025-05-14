#include "relation_base_impl.h"

#include "querybuilder.h"

#include <QtCore/qjsonarray.h>

namespace RestLink {
namespace Sql {

SingleRelationImpl::SingleRelationImpl(Relation *relation)
    : RelationImpl(relation)
{
}

QVariant SingleRelationImpl::field(const QString &name) const
{
    return m_relatedModel.field(name);
}

void SingleRelationImpl::setField(const QString &name, const QVariant &value)
{
    m_relatedModel.setField(name, value);
}

QList<Model> SingleRelationImpl::relatedModels() const
{
    return { m_relatedModel };
}

void SingleRelationImpl::setRelatedModels(const QList<Model> &models)
{
    m_relatedModel = (!models.empty() ? models.first() : Model());
}

QJsonValue SingleRelationImpl::jsonValue() const
{
    return m_relatedModel.jsonObject();
}

void SingleRelationImpl::setJsonValue(const QJsonValue &value)
{
    m_relatedModel.fill(value.toObject());
}

bool SingleRelationImpl::exists() const
{
    return m_relatedModel.exists();
}

MultipleRelationImpl::MultipleRelationImpl(Relation *relation)
    : RelationImpl(relation)
{
}

QVariant MultipleRelationImpl::field(const QString &name) const
{
    Q_UNUSED(name);
    return QVariant();
}

void MultipleRelationImpl::setField(const QString &name, const QVariant &value)
{
    Q_UNUSED(name);
    Q_UNUSED(value);
}

QList<Model> MultipleRelationImpl::relatedModels() const
{
    return m_relatedModels;
}

void MultipleRelationImpl::setRelatedModels(const QList<Model> &models)
{
    m_relatedModels = models;
}

QJsonValue MultipleRelationImpl::jsonValue() const
{
    QJsonArray data;
    for (const Model &model : m_relatedModels)
        data.append(model.jsonObject());
    return data;
}

void MultipleRelationImpl::setJsonValue(const QJsonValue &value)
{
    m_relatedModels.clear();
    if (!value.isArray())
        return;

    const QJsonArray array = value.toArray();
    for (const QJsonValue &item : array) {
        Model model = createModel();
        model.fill(item.toObject());
        m_relatedModels.append(model);
    }
}

bool MultipleRelationImpl::exists() const
{
    return false;
}

} // namespace Sql
} // namespace RestLink
