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

bool Api::isConfigured() const
{
    return !m_resources.isEmpty();
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

    return configuration;
}

void Api::configure(const QJsonObject &configuration)
{
    m_endpoints.clear();
    m_resources.clear();

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
        const QJsonObject endpointObject = endpoints.value(endpointName).toObject();

        EndpointInfo endpoint;
        endpoint.load(endpointName, endpointObject, this);
        m_endpoints.insert(endpointName, endpoint);
    }
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

void Api::cleanupApis()
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
