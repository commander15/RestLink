#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include <QtCore/qurl.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qatomic.h>

#include <QtSql/qsqldatabase.h>

namespace RestLink {
namespace Sql {

class Model;

class ModelManager final
{
public:
    ~ModelManager();

    QUrl url() const;
    QJsonObject modelDefinition(const QString &name) const;

    QJsonObject modelDefinitions(bool includeMetaData = true) const;

    bool isConfigured() const;
    QJsonObject configuration() const;
    void configure(const QJsonObject &configuration);

    QSqlDatabase database() const;

    static ModelManager *manager(const QUrl &url);
    static void cleanupManagers();

protected:
    void refModel(const Model *model);
    void unrefModel(const Model *model);

private:
    ModelManager(const QUrl &url);

    void generateModelDefinitions();
    void completeModelDefinitions();
    void fillModelDefinition(const QString &tableName, QJsonObject *definition, QSqlDatabase &db) const;

    QUrl m_url;
    QJsonObject m_modelDefinitions;
    QString m_dbConnectionName;

    QAtomicInt m_activeModels;

    static QList<ModelManager *> s_managers;

    friend class Model;
};

} // namespace Sql
} // namespace RestLink

#endif // MODELMANAGER_H
