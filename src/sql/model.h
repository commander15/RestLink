#ifndef MODEL_H
#define MODEL_H

#include "crudinterface.h"
#include "querybuilder.h"
#include "relation.h"

#include <QtCore/qshareddata.h>
#include <QtCore/qjsonobject.h>

class QSqlQuery;
class QSqlRecord;

namespace RestLink {
namespace Sql {

class ModelManager;
class Relation;

class ModelData;
class Model final: public CRUDInterface
{
public:
    Model();
    Model(const QString &table, ModelManager *manager);
    Model(const Model &other) = default;
    ~Model();

    Model &operator=(const Model &other) = default;

    QVariant primary() const;
    void setPrimary(const QVariant &value);

    QVariant field(const QString &name) const;
    void setField(const QString &name, const QVariant &value);

    void fill(const QJsonObject &data);
    void fill(const QSqlRecord &record);

    QJsonObject jsonObject() const;

    bool exists() const override;

    bool get() override;
    bool get(const QVariant &id);
    bool getByFilters(const QueryFilters &filters);

    bool load(const QStringList &relations);
    bool loadAll();

    bool insert() override;
    bool update() override;
    bool deleteData() override;

    QString tableName() const;
    QString primaryField() const;
    QJsonObject definition() const;
    QJsonObject relationDefinition(const QString &name) const;

    ModelManager *manager() const;

    static QList<Model> getMulti(const QString &table, const QueryOptions &options, ModelManager *manager);
    static int count(const QString &table, const QueryOptions &options, ModelManager *manager);

private:
    QSqlQuery exec(const QString &statement);

private:
    QSharedDataPointer<ModelData> d_ptr;

    friend class Relation;
};

class ModelData final: public QSharedData
{
public:
    QString table;
    QVariantHash data;
    QList<Relation> relations;
    ModelManager *manager = nullptr;
};

} // namespace Sql
} // namespace RestLink

#endif // MODEL_H
