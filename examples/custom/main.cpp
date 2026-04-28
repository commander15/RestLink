#include <QtCore/qcoreapplication.h>
#include <QtCore/qtimer.h>

#include <RestLink/restlink.h>
#include <RestLink/server.h>
#include <qloggingcategory.h>

#include "worker.h"

using namespace RestLink;

void listUsers(Api *api);

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    //app.addLibraryPath("../plugins");

    // Just for verbosity
    QLoggingCategory::setFilterRules("restlink.info = true");

    // We register a virtual server that will use our custom worker
    Server *server = Server::create<Worker>("Custom", { "custom" }, &app);
    RestLink::NetworkManager::registerHandler(server);

    // We test directly
    Api api;
    api.setUrl(QUrl("custom://myserver.com/api/v1"));
    listUsers(&api);

    return app.exec();
}

void listUsers(Api *api)
{
    auto handleResponse = [](Response *response) {
        qDebug() << response->method() << response->url().toString();
        if (!response->isSuccess()) {
            qDebug() << response->httpReasonPhrase();
            qDebug() << response->networkErrorString();
        } else {
            qDebug() << "HTTP " << response->httpStatusCode();
            const QStringList headers = response->headerList();
            for (const QString &header : headers)
                qDebug() << header << ": " << response->header(header);
            qDebug() << '\n';
            qDebug().noquote() << response->readBody();
        }

        if (response->isOpen())
            qDebug() << response->readAll();

        static int count = 0;
        if (++count == 4)
            qApp->quit();
    };

    const QString endpoint = "/users";

    QJsonObject user;
    user.insert("last_name", "Doe");

    user.insert("first_name", "John");
    user.insert("gender", "M");
    api->post(endpoint, user, handleResponse);

    user.insert("first_name", "Jane");
    user.insert("gender", "F");
    api->post(endpoint, user, handleResponse);

    api->get(endpoint + "/1", handleResponse);
    api->get(endpoint + "/2", handleResponse);
}
