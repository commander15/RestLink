#include "usercontroller.h"

#include <RestLink/serverrequest.h>
#include <RestLink/serverresponse.h>

#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qjsonobject.h>
#include <qjsonarray.h>

using namespace RestLink;

QString UserController::endpoint() const
{
    return QStringLiteral("/app/users");
}

void UserController::index(const ServerRequest &request, ServerResponse *response)
{
    QJsonObject o;
    o.insert("resource", request.resource());
    o.insert("identifier", request.identifier().toInt());
    o.insert("url", request.url().toString());

    response->setBody(o);
    response->setHttpStatusCode(200);
    response->complete();
}

void UserController::show(const ServerRequest &request, ServerResponse *response)
{
    QSqlQuery query(database());

    if (!query.exec("SELECT * FROM users")) {
        response->setHttpStatusCode(500);
        return;
    }

    QJsonArray data;

    while (query.next()) {
        QJsonObject object;
        object.insert("id", QJsonValue::fromVariant(query.value("id")));
        data.append(object);
    }

    response->setBody(data);
    response->setHttpStatusCode(200);
    response->complete();
}

void UserController::update(const ServerRequest &request, ServerResponse *response)
{

}

void UserController::store(const ServerRequest &request, ServerResponse *response)
{

}

void UserController::destroy(const ServerRequest &request, ServerResponse *response)
{

}

QSqlDatabase UserController::database() const
{
    return *static_cast<QSqlDatabase *>(dataSource());
}
