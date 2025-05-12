#include "relation.h"

#include "model.h"
#include "modelmanager.h"
#include "relation_api_impl.h"

namespace RestLink {
namespace Sql {

Relation::Relation()
    : m_model(nullptr)
    , m_impl(new NullRelationImpl(this))
{
}

Relation::Relation(const QString &name, Model *model)
    : m_name(name)
    , m_model(model)
{
    const QJsonObject definition = model->relationDefinition(name);

    const QString type = definition.value("type").toString();
    if (type == "HasOne") {
        m_impl.reset(new HasOneImpl(this));
    } else if (type == "BelongsTo") {
        m_impl.reset(new BelongsToImpl(this));
    } else if (type == "HasMany") {
        m_impl.reset(new HasManyImpl(this));
    } else if (type == "BelongsToMany") {
        m_impl.reset(new BelongsToManyImpl(this));
    } else {
        m_impl.reset(new NullRelationImpl(this));
    }

    m_impl->relation = this;
}

Relation::Relation(const Relation &other)
    : Relation(other.relationName(), other.root())
{
    m_impl->setRelatedModels(other.models());
}

Relation &Relation::operator=(const Relation &other)
{
    m_name = other.m_name;
    m_model = other.m_model;
    m_impl.reset(other.m_impl->clone());
    return *this;
}

QString Relation::relationName() const
{
    return m_name;
}

QJsonObject Relation::relationDefinition() const
{
    return m_model->relationDefinition(m_name);
}

QString Relation::modelName() const
{
    const QJsonObject definition = m_model->relationDefinition(relationName());
    return definition.value("table").toString();
}

bool Relation::isOwnedModel() const
{
    const QJsonObject definition = m_model->relationDefinition(relationName());
    return definition.value("owned").toBool(false);
}

QJsonObject Relation::modelDefinition() const
{
    return m_model->manager()->modelDefinition(modelName());
}

void Relation::fill(const QJsonObject &object)
{
    m_impl->fillFromRootObject(object);
}

void Relation::fill(const QSqlRecord &record)
{
    m_impl->fillFromRootRecord(record);
}

Model *Relation::root() const
{
    return m_model;
}

Model Relation::model() const
{
    const QList<Model> models = m_impl->relatedModels();
    return (!models.isEmpty() ? models.first() : Model());
}

void Relation::setModel(const Model &model)
{
    m_impl->setRelatedModels({model});
}

QList<Model> Relation::models() const
{
    return m_impl->relatedModels();
}

void Relation::setModels(const QList<Model> &models)
{
    m_impl->setRelatedModels(models);
}

QJsonValue Relation::jsonValue() const
{
    return m_impl->jsonValue();
}

void Relation::setJsonValue(const QJsonValue &value)
{
    m_impl->setJsonValue(value);
}

bool Relation::exists() const
{
    return m_impl->exists();
}

bool Relation::get()
{
    return m_impl->get();
}

bool Relation::save()
{
    if (m_operationMode == m_impl->operationMode())
        return m_impl->save();
    else
        return true;
}

bool Relation::insert()
{
    if (m_operationMode == m_impl->operationMode())
        return m_impl->insert();
    else
        return true;
}

bool Relation::update()
{
    if (m_operationMode == m_impl->operationMode())
        return m_impl->update();
    else
        return true;
}

bool Relation::deleteData()
{
    if (m_operationMode == m_impl->operationMode())
        return m_impl->deleteData();
    else
        return true;
}

void Relation::prepareOperations(OperationMode mode)
{
    m_operationMode = mode;
}

RelationImpl::RelationImpl(Relation *relation)
    : relation(relation)
{
}

QJsonObject RelationImpl::rootModelDefinition() const
{
    return relation->root()->definition();
}

QJsonObject RelationImpl::relatedModelDefinition() const
{
    return relation->modelDefinition();
}

QJsonObject RelationImpl::relationDefinition() const
{
    return relation->relationDefinition();
}

QString RelationImpl::rootPrimaryField() const
{
    const QJsonObject definition = relation->relationDefinition();
    if (definition.contains("primary"))
        return definition.value("primary").toString();

    return "id";
}

QString RelationImpl::rootForeignField() const
{
    const QJsonObject definition = relation->relationDefinition();
    if (definition.contains("foreign"))
        return definition.value("foreign").toString();

    return relation->root()->tableName().toLower() + "_id";
}

QVariant RelationImpl::rootPrimaryValue() const
{
    return relation->root()->primary();
}

QVariant RelationImpl::rootValue(const QString &name) const
{
    return relation->root()->field(name);
}

void RelationImpl::setRootValue(const QString &name, const QVariant &value)
{
    relation->root()->setField(name, value);
}

void RelationImpl::removeRootValue(const QString &name)
{
    relation->root()->setField(name, QVariant());
}

QString RelationImpl::relatedPrimaryField() const
{
    // ToDo: extract from relation definition
    return "id";
}

QString RelationImpl::relatedForeignField() const
{
    const QJsonObject definition = relation->relationDefinition();
    if (definition.contains("foreign"))
        return definition.value("foreign").toString();

    return relation->modelName().toLower() + "_id";
}

} // namespace Sql
} // namespace RestLink
