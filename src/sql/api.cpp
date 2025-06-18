#include "api.h"

#include <meta/endpointinfo.h>
#include <meta/resourceinfo.h>

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
    , m_connectionClosable(true)
    , m_autoConfigured(true)
    , m_activeModels(0)
{
    static unsigned int connectionId = 0;

    const QString driverName = 'Q' + url.scheme().toUpper();
    m_dbConnectionName = QStringLiteral("RestLink_%1").arg(connectionId++);

    QSqlDatabase db = QSqlDatabase::addDatabase(driverName, m_dbConnectionName);
    db.setHostName(url.host());
    db.setPort(url.port());
    db.setUserName(url.userName());
    db.setPassword(url.password());

    if (url.scheme() == "sqlite") {
        if (url.path().startsWith("memory"))
            db.setDatabaseName(":memory:");
        else
            db.setDatabaseName(url.path());
    } else
        db.setDatabaseName(url.path().mid(1));

    reset();

    s_apis.insert(url, this);
}

Api::~Api()
{
    if (!s_shutingDown && QSqlDatabase::contains(m_dbConnectionName))
        QSqlDatabase::removeDatabase(m_dbConnectionName);

    s_apis.remove(m_url);
}

QUrl Api::url() const
{
    return m_url;
}

bool Api::canCloseConnection() const
{
    return m_connectionClosable;
}

void Api::setConnectionClosable(bool closeable)
{
    m_connectionClosable = closeable;
}

bool Api::isConfigured() const
{
    return !m_resources.isEmpty();
}

bool Api::isAutoConfigured() const
{
    return m_autoConfigured;
}

QJsonObject Api::configuration() const
{
    QJsonObject configuration;

    QJsonObject endpoints;
    for (const EndpointInfo &info : m_endpoints) {
        QJsonObject endpoint;
        info.save(&endpoint);
        endpoints.insert(info.name(), endpoint);
    }
    configuration.insert("endpoints", endpoints);

    QJsonObject resources;
    for (const ResourceInfo &info : m_resources) {
        QJsonObject resource;
        info.save(&resource);
        resources.insert(info.name(), resource);
    }
    configuration.insert("resources", resources);

    return configuration;
}

void Api::configure(const QJsonObject &configuration, const QHash<QString, QString> &options)
{
    m_endpoints.clear();
    m_resources.clear();

    if (!options.isEmpty()) {
        QSqlDatabase db = QSqlDatabase::database(m_dbConnectionName, false);
        if (db.isOpen())
            db.close();

        if (options.contains("DATABASE_NAME"))
            db.setDatabaseName(options.value("DATABASE_NAME"));

        if (options.contains("CONNECT_OPTIONS"))
            db.setConnectOptions(options.value("CONNECT_OPTIONS"));
    }

    // Loading resources
    const QJsonObject resources = configuration.value("resources").toObject();
    const QStringList resourceNames = resources.keys();
    for (const QString &resourceName : resourceNames) {
        ResourceInfo resource;
        resource.load(resourceName, resources.value(resourceName).toObject(), this);

        if (resource.isValid()) {
            m_resources.insert(resourceName, resource);

            const EndpointInfo endpoint = EndpointInfo::fromResource(resource);
            m_endpoints.insert(endpoint.name(), endpoint);
        }
    }

    // Loading endpoints
    const QJsonObject endpoints = configuration.value("endpoints").toObject();
    const QStringList &endpointNames = endpoints.keys();
    for (const QString &endpointName : endpointNames) {
        EndpointInfo endpoint;
        endpoint.load(endpointName, endpoints.value(endpointName).toObject(), this);
        m_endpoints.insert(endpointName, endpoint);
    }

    resetIdleTime();

    m_autoConfigured = false;
}

void Api::reset()
{
    m_endpoints.clear();
    m_resources.clear();

    QSqlDatabase db = database();

    const QStringList tables = db.tables();
    for (const QString &tableName : tables) {
        QJsonObject object;
        object.insert("table", tableName);

        EndpointInfo endpoint;
        endpoint.load('/' + tableName.toLower(), object, this);
        m_endpoints.insert(endpoint.name(), endpoint);

        if (endpoint.hasResource())
            m_resources.insert(endpoint.name(), endpoint.resource());
    }

    resetIdleTime();

    m_autoConfigured = true;
}

EndpointInfo Api::endpointInfo(const QString &name) const
{
    return m_endpoints.value(name);
}

ResourceInfo Api::resourceInfo(const QString &name) const
{
    return m_resources.value(name);
}

ResourceInfo Api::resourceInfoByTable(const QString &table) const
{
    auto it = std::find_if(m_resources.begin(), m_resources.end(), [&table](const ResourceInfo &info) {
        return info.table() == table;
    });

    return (it != m_resources.end() ? *it : ResourceInfo());
}

QStringList Api::resourceNames() const
{
    return m_resources.keys();
}

qint64 Api::idleTime() const
{
    const QDateTime now = QDateTime::currentDateTime();
    return now.secsTo(m_lastUsedTime);
}

void Api::resetIdleTime()
{
    m_lastUsedTime = QDateTime::currentDateTime();
}

void Api::closeDatabase()
{
    QSqlDatabase db = QSqlDatabase::database(m_dbConnectionName, false);
    if (db.isOpen())
        db.close();
}

QSqlDatabase Api::database() const
{
    return QSqlDatabase::database(m_dbConnectionName, true);
}

bool Api::hasApi(const QUrl &url)
{
    if (!url.isValid())
        return false;

    auto it = std::find_if(s_apis.begin(), s_apis.end(), [&url](const Api *manager) {
        return manager->m_url == url;
    });

    return (it != s_apis.end());
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

int Api::apiCount()
{
    return s_apis.count();
}

void Api::purgeApis(int atLeast, bool remove)
{
    if (atLeast < 0)
        atLeast = 0;
    else if (atLeast > s_apis.size())
        atLeast = s_apis.size();

    int closed = 0;
    auto closeConnections = [&closed, &remove](const QList<Api *> apis, bool force) {
        for (Api *api : apis) {
            // We close database connections which were unused during at least 30 minutes
            if (force || api->idleTime() > 1800) {
                closed++;
                if (remove)
                    delete api;
                else if (api->canCloseConnection())
                    api->closeDatabase();
                else
                    closed--;
            }
        }
    };

    closeConnections(s_apis.values(), false);
    if (closed >= atLeast)
        return;

    QList<Api *> apis = s_apis.values();
    std::sort(apis.begin(), apis.end(), [](Api *a1, Api *a2) {
        if (a1->isAutoConfigured() != a2->isAutoConfigured())
            return a1->isAutoConfigured(); // auto-configured first
        return a1->idleTime() > a2->idleTime(); // then by idle time
    });

    apis.remove(0, atLeast);
    closeConnections(apis, true);
}

void Api::cleanupApis()
{
    s_shutingDown = true;
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
bool Api::s_shutingDown(false);

} // namespace Sql
} // namespace RestLink
