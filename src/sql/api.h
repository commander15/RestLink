#ifndef API_H
#define API_H

#include <global.h>

#include <QtCore/qurl.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qatomic.h>

#include <QtSql/qsqldatabase.h>

namespace RestLink {
namespace Sql {

class EndpointInfo;
class ResourceInfo;
class Model;

class SQL_EXPORT Api final
{
public:
    ~Api();

    QUrl url() const;

    bool canCloseConnection() const;
    void setConnectionClosable(bool closeable);

    bool isConfigured() const;
    bool isAutoConfigured() const;
    QJsonObject configuration() const;
    void configure(const QJsonObject &configuration);
    void reset();

    EndpointInfo endpointInfo(const QString &name) const;
    ResourceInfo resourceInfo(const QString &name) const;
    ResourceInfo resourceInfoByTable(const QString &table) const;
    QStringList resourceNames() const;

    qint64 idleTime() const;
    void resetIdleTime();

    void closeDatabase();
    QSqlDatabase database() const;

    static bool hasApi(const QUrl &url);
    static Api *api(const QUrl &url);
    static int apiCount();
    static void purgeApis(int atLeast = 0, bool remove = false);
    static void cleanupApis();

protected:
    void refModel(const Model *model);
    void unrefModel(const Model *model);

private:
    Api(const QUrl &url);

    const QUrl m_url;
    bool m_connectionClosable;

    QHash<QString, EndpointInfo> m_endpoints;
    QHash<QString, ResourceInfo> m_resources;
    bool m_autoConfigured;
    QDateTime m_lastUsedTime;
    QString m_dbConnectionName;

    QAtomicInt m_activeModels;

    static QHash<QUrl, Api *> s_apis;

    friend class Model;
};

} // namespace Sql
} // namespace RestLink

#endif // API_H
