#ifndef MODEL_H
#define MODEL_H

#include "crudinterface.h"
#include "relation.h"

#include <utils/querybuilder.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qjsonobject.h>

class QSqlQuery;
class QSqlRecord;
class QSqlError;

namespace RestLink {
namespace Sql {

class Api;
class Relation;

class ModelData;
class Model final: public CRUDInterface
{
public:
    Model();
    Model(const QString &resource, Api *manager);
    Model(const Model &other);
    ~Model();

    Model &operator=(const Model &other);

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

    bool loadAll();
    bool load(const QStringList &relations);

    bool insert() override;
    bool update() override;
    bool deleteData() override;

    const QSqlQuery &lastQuery() const;

    RelationInfo relationInfo(const QString &name) const;
    ResourceInfo resourceInfo() const;

    Api *api() const;

    static QList<Model> getMulti(const QString &resource, const QueryOptions &options, Api *api, QSqlQuery *query);
    static int count(const QString &resource, const QueryOptions &options, Api *api);

private:
    QSqlQuery exec(const QString &statement);

private:
    QSharedDataPointer<ModelData> d_ptr;

    friend class Relation;
    friend class RelationImpl;
};

} // namespace Sql
} // namespace RestLink

#endif // MODEL_H
