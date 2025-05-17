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

    bool isConfigured() const;
    QJsonObject configuration() const;
    void configure(const QJsonObject &configuration);
    void reset();

    EndpointInfo endpointInfo(const QString &name) const;
    ResourceInfo resourceInfo(const QString &name) const;
    ResourceInfo resourceInfoByTable(const QString &table) const;
    QStringList resourceNames() const;

    QSqlDatabase database() const;

    static bool hasApi(const QUrl &url);
    static Api *api(const QUrl &url);
    static void cleanupApis();

protected:
    void refModel(const Model *model);
    void unrefModel(const Model *model);

private:
    Api(const QUrl &url);

    const QUrl m_url;
    QHash<QString, EndpointInfo> m_endpoints;
    QHash<QString, ResourceInfo> m_resources;
    QString m_dbConnectionName;

    QAtomicInt m_activeModels;

    static QHash<QUrl, Api *> s_apis;

    friend class Model;
};

} // namespace Sql
} // namespace RestLink

#endif // API_H
