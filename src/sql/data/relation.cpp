#include "relation.h"

#include "model.h"

#include <api.h>
#include <meta/relationinfo.h>

#include <relations/relation_api_impl.h>

#include <QtSql/qsqlquery.h>

namespace RestLink {
namespace Sql {

Relation::Relation()
    : m_model(nullptr)
    , m_impl(new NullRelationImpl(this))
{
}

Relation::Relation(const QString &name, Model *model)
    : m_info(model->relationInfo(name))
    , m_model(model)
{
    switch (static_cast<Type>(m_info.type())) {
    case Type::HasOne:
        m_impl.reset(new HasOneImpl(this));
        break;

    case Type::BelongsToOne:
        m_impl.reset(new BelongsToOneImpl(this));
        break;

    case Type::HasMany:
        m_impl.reset(new HasManyImpl(this));
        break;

    case Type::BelongsToMany:
        m_impl.reset(new BelongsToManyThroughImpl(this));
        break;

    default:
        m_impl.reset(new NullRelationImpl(this));
        break;
    }

    m_impl->relation = this;
    m_impl->root = model;
    m_impl->rootResource = model->resourceInfo();
    m_impl->info = m_info;
    m_impl->foreignResource = model->api()->resourceInfoByTable(m_info.table());

    const ResourceInfo res = model->resourceInfo();
}

Relation::Relation(const Relation &other)
    : Relation(other.relationName(), other.root())
{
    m_impl->setRelatedModels(other.models());
}

Relation &Relation::operator=(const Relation &other)
{
    m_info = other.m_info;
    m_model = other.m_model;
    m_impl.reset(other.m_impl->clone());
    return *this;
}

QString Relation::relationName() const
{
    return m_info.name();
}

QString Relation::modelName() const
{
    return m_info.table();
}

bool Relation::isOwnedModel() const
{
    return m_info.owned();
}

void Relation::fill(const QJsonValue &value)
{
    m_impl->fillFromJson(value);
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

Model RelationImpl::createModel() const
{
    return Model(foreignResource.name(), root->api());
}

QSqlQuery RelationImpl::exec(const QString &statement)
{
    return root->exec(statement);
}

} // namespace Sql
} // namespace RestLink
