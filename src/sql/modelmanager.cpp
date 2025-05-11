#include "modelmanager.h"

#include <RestLink/debug.h>

#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonvalue.h>

#include <QtSql/qsqlindex.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>

namespace RestLink {
namespace Sql {

ModelManager::ModelManager(const QUrl &url)
    : m_url(url)
{
    static unsigned int connectionId = 0;

    const QString driverName = 'Q' + url.scheme().toUpper();
    m_dbConnectionName = QStringLiteral("RestLink_%1").arg(connectionId++);

    QSqlDatabase db = QSqlDatabase::addDatabase(driverName, m_dbConnectionName);
    db.setHostName(url.host());
    db.setPort(url.port());
    db.setUserName(url.userName());
    db.setPassword(url.password());
    db.setDatabaseName(url.path());

    s_managers.append(this);
}

ModelManager::~ModelManager()
{
    if (m_activeModels == 0)
        QSqlDatabase::removeDatabase(m_dbConnectionName);
    else
        restlinkWarning() << "(Internal Error) - Can't close connection to " << m_url.toString();

    s_managers.removeOne(this);
}

QUrl ModelManager::url() const
{
    return m_url;
}

QJsonObject ModelManager::modelDefinition(const QString &name) const
{
    if (m_modelDefinitions.isEmpty())
        manager(url())->generateModelDefinitions();
    return m_modelDefinitions.value(name).toObject();
}

QJsonObject ModelManager::modelDefinitions(bool includeMetaData) const
{
    QJsonObject root;

    QJsonObject definitions;
    {
        QSqlDatabase db = database();

        const QStringList tables = db.tables();
        for (const QString &tableName : tables) {
            QJsonObject table;
            if (includeMetaData)
                fillModelDefinition(tableName, &table, db);
            else
                table.insert("table", tableName);
            definitions.insert(tableName, table);
        }
    }
    root.insert("models", definitions);

    return root;
}

bool ModelManager::isConfigured() const
{
    return !m_modelDefinitions.isEmpty();
}

QJsonObject ModelManager::configuration() const
{
    if (m_modelDefinitions.isEmpty())
        manager(url())->generateModelDefinitions();

    QJsonObject configuration;
    configuration.insert("models", m_modelDefinitions);
    return configuration;
}

void ModelManager::configure(const QJsonObject &configuration)
{
    m_modelDefinitions = configuration.value("models").toObject();

    if (m_modelDefinitions.isEmpty())
        generateModelDefinitions();
    else
        completeModelDefinitions();
}

QSqlDatabase ModelManager::database() const
{
    return QSqlDatabase::database(m_dbConnectionName, true);
}

ModelManager *ModelManager::manager(const QUrl &url)
{
    if (!url.isValid())
        return nullptr;

    auto it = std::find_if(s_managers.begin(), s_managers.end(), [&url](const ModelManager *manager) {
        return manager->m_url == url;
    });

    if (it != s_managers.end())
        return *it;

    return new ModelManager(url);
}

void ModelManager::cleanupManagers()
{
    while (!s_managers.empty())
        delete s_managers.takeFirst();
}

void ModelManager::refModel(const Model *model)
{
    Q_UNUSED(model);
    m_activeModels.ref();
}

void ModelManager::unrefModel(const Model *model)
{
    Q_UNUSED(model);
    m_activeModels.deref();
}

void ModelManager::generateModelDefinitions()
{
    QJsonObject definitions;
    QSqlDatabase db = database();

    const QStringList tables = db.tables();
    for (const QString &table : tables) {
        QJsonObject definition;
        fillModelDefinition(table, &definition, db);
        definitions.insert(table, definition);
    }

    m_modelDefinitions = definitions;
}

void ModelManager::completeModelDefinitions()
{
    QSqlDatabase db = database();

    const QStringList tables = m_modelDefinitions.keys();
    for (const QString &tableName : tables) {
        QJsonObject definition = m_modelDefinitions.value(tableName).toObject();
        fillModelDefinition(tableName, &definition, db);
        m_modelDefinitions.insert(tableName, definition);
    }
}

void ModelManager::fillModelDefinition(const QString &tableName, QJsonObject *definition, QSqlDatabase &db) const
{
    definition->insert("table", tableName);

    QSqlIndex primaryIndex = db.primaryIndex(tableName);
    if (primaryIndex.count() == 1)
        definition->insert("primary", primaryIndex.fieldName(0));

    QSqlRecord record = db.record(tableName);
    QJsonArray fields;
    for (int i = 0; i < record.count(); ++i) {
        QSqlField field = record.field(i);
        QJsonObject fieldDef;
        fieldDef.insert("name", field.name());
        fieldDef.insert("type", field.metaType().name());
        fieldDef.insert("required", field.requiredStatus() == QSqlField::Required);
        fieldDef.insert("length", field.length());
        fieldDef.insert("auto_increment", field.isAutoValue());
        fields.append(fieldDef);
    }

    definition->insert("fields", fields);
}

QList<ModelManager *> ModelManager::s_managers;

} // namespace Sql
} // namespace RestLink
