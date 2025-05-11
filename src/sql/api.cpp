#include "api.h"

#include "jsonutils.h"
#include "modelmanager.h"

#include <QtCore/qfile.h>

namespace RestLink {
namespace Sql {

Api::Api(QObject *parent)
    : RestLink::Server(Synchronous, parent)
{
}

Api::~Api()
{
    ModelManager::cleanupManagers();
}

QString Api::handlerName() const
{
    return QStringLiteral("SQL-API");
}

QStringList Api::supportedSchemes() const
{
    static QStringList schemes;
    if (schemes.isEmpty()) {
        schemes = QSqlDatabase::drivers();
        std::transform(schemes.begin(), schemes.end(), schemes.begin(), [](const QString &driver) {
            return driver.mid(1).toLower();
        });
    }
    return schemes;
}

bool Api::init()
{
    return true;
}

void Api::cleanup()
{
    // No-op
}

bool Api::maintain()
{
    return true;
}

void Api::processRequest(Method method, const ServerRequest &request, ServerResponse *response)
{
    ModelManager *manager = ModelManager::manager(request.baseUrl());
    if (!manager) {
        response->setHttpStatusCode(404);
        response->complete();
        return;
    }

    if (request.hasQueryParameter("configuration-file")) {
        QFile file(request.queryParameterValues("configuration-file").constFirst().toString());
        if (file.open(QIODevice::ReadOnly)) {
            QJsonObject config = JsonUtils::objectFromRawData(file.readAll(), QJsonObject());
            if (!config.isEmpty())
                manager->configure(config);
        }
    }

    if (request.endpoint() == "/configuration") {
        switch (method) {
        case PostMethod:
            manager->configure(JsonUtils::objectFromRawData(request.body().data(), QJsonObject()));

        case GetMethod:
            response->setBody(manager->configuration());
            response->setHttpStatusCode(200);
            break;

        default:
            response->setHttpStatusCode(404);
            break;
        }

        response->complete();
        return;
    }

    if (request.endpoint() == "/tables" && method == GetMethod) {
        bool includeMetaData = true;
        if (request.hasQueryParameter("meta"))
            includeMetaData = request.queryParameterValues("meta").constFirst().toBool();

        response->setHttpStatusCode(200);
        response->setBody(manager->modelDefinitions(includeMetaData));
        response->complete();
        return;
    }

    m_defaultController.setManager(manager);
    if (m_defaultController.canProcessRequest(request)) {
        m_defaultController.processRequest(method, request, response);
        return;
    }

    response->setHttpStatusCode(404);
    response->complete();
}

} // namespace Sql
} // namespace RestLink
