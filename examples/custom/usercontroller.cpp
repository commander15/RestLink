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
    return QStringLiteral("/users");
}

void UserController::index(ServerRequest &request, ServerResponse *response)
{
    QJsonObject o;
    o.insert("resource", request.resource());
    o.insert("identifier", request.identifier().toInt());
    o.insert("url", request.url().toString());

    response->setBody(o);
    response->setHttpStatusCode(200);
    response->complete();
}

void UserController::show(ServerRequest &request, ServerResponse *response)
{
    QSqlQuery query(database());

    if (!query.prepare("SELECT * FROM users WHERE id=?")) {
        response->setHttpStatusCode(500);
        response->complete();
        return;
    }

    query.bindValue(0, request.identifier().toInt());
    if (!query.exec()) {
        response->setHttpStatusCode(500);
        response->complete();
        return;
    }

    QJsonArray data;

    while (query.next()) {
        QJsonObject object;
        object.insert("id", QJsonValue::fromVariant(query.value("id")));
        object.insert("first_name", QJsonValue::fromVariant(query.value("first_name")));
        object.insert("last_name", QJsonValue::fromVariant(query.value("last_name")));
        data.append(object);
    }

    response->setBody(data);
    response->setHttpStatusCode(200);
    response->complete();
}

void UserController::update(ServerRequest &request, ServerResponse *response)
{
}

void UserController::store(ServerRequest &request, ServerResponse *response)
{
    const QJsonObject data = request.body().jsonObject();

    QSqlQuery query(database());
    query.prepare("INSERT INTO Users(first_name, last_name, gender) VALUES(?, ?, ?)");
    query.bindValue(0, data.value("first_name").toVariant());
    query.bindValue(1, data.value("last_name").toVariant());
    query.bindValue(2, data.value("gender").toVariant());

    if (query.exec()) {
        response->setHttpStatusCode(201);
    } else {
        response->setHttpStatusCode(500);
    }

    response->complete();
}

void UserController::destroy(ServerRequest &request, ServerResponse *response)
{
}

QSqlDatabase UserController::database() const
{
    return QSqlDatabase::database(*static_cast<const QString *>(dataSource()));
}
