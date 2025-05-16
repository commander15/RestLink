#ifndef RELATION_H
#define RELATION_H

#include <data/crudinterface.h>
#include <meta/relationinfo.h>
#include <meta/resourceinfo.h>

#include <QtCore/qscopedpointer.h>
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <QtCore/qjsonvalue.h>

class QSqlQuery;
class QSqlRecord;

namespace RestLink {
namespace Sql {

class Model;

class RelationData;
class RelationImpl;
class Relation : public CRUDInterface
{
public:
    enum OperationMode {
        PreProcessing,
        PostProcessing
    };

    enum Type {
        HasOne = 0,
        BelongsTo = 1,
        HasMany = 2,
        BelongsToMany = 3,

        Null = -1
    };

    Relation();
    Relation(const QString &name, Model *model);
    Relation(const Relation &other);
    ~Relation() = default;

    Relation &operator=(const Relation &other);

    QString relationName() const;

    QString modelName() const;
    bool isOwnedModel() const;

    void fill(const QJsonObject &object);
    void fill(const QSqlRecord &record);

    Model *root() const;

    Model model() const;
    void setModel(const Model &model);

    QList<Model> models() const;
    void setModels(const QList<Model> &models);

    QJsonValue jsonValue() const;
    void setJsonValue(const QJsonValue &value);

    bool exists() const override;
    bool get() override;
    bool save() override;
    bool insert() override;
    bool update() override;
    bool deleteData() override;

    void prepareOperations(OperationMode mode);

private:
    RelationInfo m_info;
    Model *m_model;
    QScopedPointer<RelationImpl> m_impl;
    OperationMode m_operationMode;
};

class RelationImpl : public CRUDInterface
{
public:
    RelationImpl(Relation *relation);
    virtual ~RelationImpl() = default;

    virtual QVariant field(const QString &name) const = 0;
    virtual void setField(const QString &name, const QVariant &value) = 0;

    virtual void fillFromRootObject(const QJsonObject &object) {}
    virtual void fillFromRootRecord(const QSqlRecord &record) {};

    virtual QList<Model> relatedModels() const = 0;
    virtual void setRelatedModels(const QList<Model> &models) = 0;

    virtual QJsonValue jsonValue() const = 0;
    virtual void setJsonValue(const QJsonValue &value) = 0;

    virtual Relation::OperationMode operationMode() const
    { return Relation::OperationMode::PreProcessing; }
    virtual Relation::Type relationType() const = 0;

    virtual RelationImpl *clone() const = 0;

protected:
    QVariant rootPrimaryValue() const;
    QVariant rootValue(const QString &name) const;
    void setRootValue(const QString &name, const QVariant &value);
    void removeRootValue(const QString &name);

    Model createModel() const;

    QSqlQuery exec(const QString &statement);

    Model *root;
    ResourceInfo rootResource;
    ResourceInfo foreignResource;

    Relation *relation;
    RelationInfo info;

    friend class Relation;
};

class NullRelationImpl final : public RelationImpl
{
public:
    NullRelationImpl(Relation *relation) : RelationImpl(relation) {}

    QVariant field(const QString &name) const override { error(); return QVariant(); }
    void setField(const QString &name, const QVariant &value) override { error(); }

    void fillFromRootObject(const QJsonObject &object) override { error(); }
    void fillFromRootRecord(const QSqlRecord &record) override { error(); }

    QList<Model> relatedModels() const override { error(); return {}; }
    void setRelatedModels(const QList<Model> &models) override { error(); }

    QJsonValue jsonValue() const override { error(); return QJsonValue(); }
    void setJsonValue(const QJsonValue &value) override { error(); };

    bool exists() const override { return error(); }
    bool get() override { return error(); }
    bool insert() override { return error(); }
    bool update() override { return error(); }
    bool deleteData() override { return error(); }

    bool error() const
    { qDebug("Null relation got called()"); return false; }

    Relation::Type relationType() const override
    { return Relation::Type::Null; }

    NullRelationImpl *clone() const override
    { return new NullRelationImpl(relation); }
};

} // namespace Sql
} // namespace RestLink

#endif // RELATION_H
