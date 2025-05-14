#include "api.h"

#include "resourceinfo.h"

#include <RestLink/debug.h>

#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonvalue.h>

#include <QtSql/qsqlindex.h>
#include <QtSql/qsqlrecord.h>
#include <QtSql/qsqlfield.h>

namespace RestLink {
namespace Sql {

Api::Api(const QUrl &url)
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

    reset();

    s_apis.insert(url, this);
}

Api::~Api()
{
    if (m_activeModels == 0)
        QSqlDatabase::removeDatabase(m_dbConnectionName);
    else
        restlinkWarning() << "(Internal Error) - Can't close connection to " << m_url.toString();

    s_apis.remove(m_url);
}

QUrl Api::url() const
{
    return m_url;
}

ResourceInfo Api::resourceInfo(const QString &name) const
{
    return m_resourceInfos.value(name);
}

ResourceInfo Api::resourceInfoByTable(const QString &table) const
{
    auto it = std::find_if(m_resourceInfos.begin(), m_resourceInfos.end(), [&table](const ResourceInfo &info) {
        return info.table() == table;
    });

    return (it != m_resourceInfos.end() ? *it : ResourceInfo());
}

QStringList Api::resourceNames() const
{
    return m_resourceInfos.keys();
}

bool Api::isConfigured() const
{
    return !m_resourceInfos.isEmpty();
}

QJsonObject Api::configuration() const
{
    QJsonObject configuration;

    QJsonObject resources;
    for (const ResourceInfo &info : m_resourceInfos) {
        QJsonObject resource;
        info.save(&resource);
        resources.insert(info.name(), resource);
    }
    configuration.insert("resources", resources);

    return configuration;
}

void Api::configure(const QJsonObject &configuration)
{
    m_resourceInfos.clear();

    const QJsonObject resources = configuration.value("resources").toObject();
    const QStringList &resourceNames = resources.keys();
    for (const QString &resourceName : resourceNames) {
        const QJsonObject resourceObject = resources.value(resourceName).toObject();

        ResourceInfo resource;
        resource.load(resourceName, resourceObject, this);

        if (true)
            m_resourceInfos.insert(resourceName, resource);
    }
}

void Api::reset()
{
    m_resourceInfos.clear();

    QSqlDatabase db = database();

    const QStringList tables = db.tables();
    for (const QString &tableName : tables) {
        QJsonObject object;
        object.insert("table", tableName);

        ResourceInfo resource;
        resource.load(tableName.toLower(), object, this);

        if (true)
            m_resourceInfos.insert(resource.name(), resource);
    }
}

QSqlDatabase Api::database() const
{
    return QSqlDatabase::database(m_dbConnectionName, true);
}

Api *Api::api(const QUrl &url)
{
    if (!url.isValid())
        return nullptr;

    auto it = std::find_if(s_apis.begin(), s_apis.end(), [&url](const Api *manager) {
        return manager->m_url == url;
    });

    if (it != s_apis.end())
        return *it;

    return new Api(url);
}

void Api::cleanupManagers()
{
    const QList<QUrl> urls = s_apis.keys();
    for (const QUrl &url : urls)
        delete s_apis.take(url);
}

void Api::refModel(const Model *model)
{
    Q_UNUSED(model);
    m_activeModels.ref();
}

void Api::unrefModel(const Model *model)
{
    Q_UNUSED(model);
    m_activeModels.deref();
}

QHash<QUrl, Api *> Api::s_apis;

} // namespace Sql
} // namespace RestLink
