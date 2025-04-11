#include "sqlserver.h"

#include <RestLink/serverresponse.h>
#include <RestLink/resourcecontroller.h>

#define RESTLINK_SQL_HEADER "X-SQL-QUERY"

namespace RestLink {

SqlServer::SqlServer(QObject *parent)
    : Server(Synchronous, parent)
    , m_manager(new SqlDatabaseManager())
{
}

SqlServer::~SqlServer()
{
    delete m_manager;
}

QString SqlServer::handlerName() const
{
    return QStringLiteral("SqlHandler");
}

QStringList SqlServer::supportedSchemes() const
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

bool SqlServer::init()
{
    return true;
}

void SqlServer::cleanup()
{
}

bool SqlServer::maintain()
{
    return true;
}

AbstractController *SqlServer::findController(const ServerRequest &request) const
{
    AbstractController *controller = Server::findController(request);
    if (controller)
        return controller;

    return m_manager->database(request.baseUrl());
}

void SqlServer::prepareController(AbstractController *controller, ApiBase::Operation operation, const ServerRequest &request, ServerResponse *response)
{
    SqlDatabase *db = m_manager->database(request.baseUrl());
    if (db)
        controller->setDataSource(static_cast<QSqlDatabase *>(db));
}

void SqlServer::processRequest(ApiBase::Operation operation, const ServerRequest &request, ServerResponse *response)
{
    AbstractController *controller = findController(request);
    if (controller) {
        prepareController(controller, operation, request, response);
        controller->processRequest(operation, request, response);
        return;
    }

    SqlDatabase *db = m_manager->database(request.baseUrl());
    if (!db) {
        response->setHttpStatusCode(500);
        response->complete();
        return;
    }

    db->processRequest(operation, request, response);
}

void SqlServer::processInternalRequest(ApiBase::Operation operation, const ServerRequest &request, ServerResponse *response)
{
    Server::processInternalRequest(operation, request, response);

    if (request.endpoint() == "/restlink/configuration") {
        // Update databases configuration here !
    }
}

} // namespace RestLink
