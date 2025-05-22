#ifndef MODEL_H
#define MODEL_H

#include "crudinterface.h"
#include "relation.h"

#include <global.h>
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
class SQL_EXPORT Model final: public CRUDInterface
{
public:
    enum FillMode {
        NormalFill,
        FullFill
    };

    Model();
    Model(const QString &resource, Api *api);
    Model(const ResourceInfo &resource, Api *api);
    Model(const Model &other);
    ~Model();

    Model &operator=(const Model &other);

    QVariant primary() const;
    void setPrimary(const QVariant &value);

    QDateTime createdAt() const;
    QDateTime updatedAt() const;

    QVariant field(const QString &name) const;
    void setField(const QString &name, const QVariant &value);

    Relation relation(const QString &name) const;

    void fill(const QJsonObject &data, FillMode mode = FullFill);
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

    QJsonObject lastQuery() const;
    QJsonObject lastError() const;

    RelationInfo relationInfo(const QString &name) const;
    ResourceInfo resourceInfo() const;

    bool isValid() const;

    Api *api() const;

    static QList<Model> getMulti(const QString &resource, const QueryOptions &options, Api *api, bool *success);
    static QList<Model> getMulti(const ResourceInfo &resource, const QueryOptions &options, Api *api, bool *success);

    static QList<Model> getMulti(const QString &resource, const QueryOptions &options, Api *api, QSqlError *error);
    static QList<Model> getMulti(const ResourceInfo &resource, const QueryOptions &options, Api *api, QSqlError *error);

    static QList<Model> getMulti(const QString &resource, const QueryOptions &options, Api *api, QSqlQuery *query);
    static QList<Model> getMulti(const ResourceInfo &resource, const QueryOptions &options, Api *api, QSqlQuery *query);

    static int count(const QString &resource, const QueryOptions &options, Api *api);
    static int count(const ResourceInfo &resource, const QueryOptions &options, Api *api);

private:
    QSqlQuery exec(const QString &statement);

private:
    QExplicitlySharedDataPointer<ModelData> d_ptr;

    friend class Relation;
    friend class RelationImpl;
};

} // namespace Sql
} // namespace RestLink

#endif // MODEL_H
