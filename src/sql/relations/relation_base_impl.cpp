#include "relation_base_impl.h"
#include "utils/queryrunner.h"

#include <utils/querybuilder.h>

#include <QtCore/qjsonarray.h>

namespace RestLink {
namespace Sql {

SingleRelationImpl::SingleRelationImpl(Relation *relation)
    : RelationImpl(relation)
{
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
    if (!m_relatedModel.isValid())
        m_relatedModel = createModel();
    m_relatedModel.fill(value.toObject());
}

bool SingleRelationImpl::exists() const
{
    return m_relatedModel.exists();
}

bool SingleRelationImpl::get()
{
    const QStringList relations = relation->loadableRelations();
    return (relations.isEmpty() ? true : m_relatedModel.load(relations));
}

MultipleRelationImpl::MultipleRelationImpl(Relation *relation)
    : RelationImpl(relation)
{
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

bool MultipleRelationImpl::get()
{
    const QStringList relations = relation->loadableRelations();
    if (relations.isEmpty())
        return true;

    for (Model &model : m_relatedModels)
        if (!model.load(relations))
            return false;

    return true;
}

bool MultipleRelationImpl::save()
{
    for (Model &model : m_relatedModels)
        if (!model.save())
            return false;
    return true;
}

bool MultipleRelationImpl::insert()
{
    for (Model &model : m_relatedModels)
        if (!model.insert())
            return false;
    return true;
}

bool MultipleRelationImpl::update()
{
    return save();
}

bool MultipleRelationImpl::deleteData()
{
    for (Model &model : m_relatedModels)
        if (!model.deleteData())
            return false;
    return true;
}

QStringList MultipleRelationImpl::formatedRelatedIds() const
{
    QStringList ids;
    ids.resize(m_relatedModels.size());
    std::transform(m_relatedModels.begin(), m_relatedModels.end(), ids.begin(), [](const Model &model) {
        return QueryBuilder::formatValue(model.primary(), model.api());
    });
    return ids;
}

QVariant MultipleThroughRelationImpl::field(const QString &name, int index) const
{
    if (index >= 0 && index < m_pivots.size())
        return m_pivots.at(index).value(name);
    return QVariant();
}

void MultipleThroughRelationImpl::setField(const QString &name, const QVariant &value, int index)
{
    if (index >= 0 && index < m_pivots.size())
        m_pivots[index].insert(name, value);
}

QJsonValue MultipleThroughRelationImpl::jsonValue() const
{
    QJsonArray data;
    for (int i(0); i < m_relatedModels.size(); ++i) {
        QJsonObject object = m_relatedModels.at(i).jsonObject();

        const QVariantHash pivot = m_pivots.at(i);
        if (!pivot.isEmpty())
            object.insert("pivot", QJsonObject::fromVariantHash(pivot));

        data.append(object);
    }
    return data;
}

void MultipleThroughRelationImpl::setJsonValue(const QJsonValue &value)
{
    m_relatedModels.clear();
    m_pivots.clear();

    if (!value.isArray())
        return;

    const QJsonArray array = value.toArray();
    for (int i(0); i < array.size(); ++i) {
        const QJsonObject object = array.at(i).toObject();
        if (object.contains("pivot"))
            m_pivots.append(object.value("pivot").toObject().toVariantHash());

        Model model = createModel();
        model.fill(object);
        m_relatedModels.append(model);
    }
}

QJsonValue PivotBasedMultipleRelationImpl::jsonValue() const
{
    QJsonArray data;

    for (int i(0); i < m_relatedModels.size(); ++i) {
        QJsonObject object = m_relatedModels.at(i).jsonObject();
        if (i < m_pivots.size())
            object.insert("pivot", m_pivots.at(i).jsonObject());
        data.append(object);
    }

    return data;
}

void PivotBasedMultipleRelationImpl::setJsonValue(const QJsonValue &value)
{
    const QJsonArray data = value.toArray();

    for (int i(0); i < data.size(); ++i) {
        const QJsonObject object = data.at(i).toObject();

    }
}

ResourceInfo PivotBasedMultipleRelationImpl::pivotResource() const
{
    const QString table = info.pivot();
    return ResourceInfo::pivotResourceInfo("pivot", table, rootResource, foreignResource, root->api());
}

} // namespace Sql
} // namespace RestLink
