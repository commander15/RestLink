#include <QtCore/qcoreapplication.h>
#include <QtCore/qtimer.h>

#include <RestLink/restlink.h>
#include <qloggingcategory.h>

#include "usercontroller.h"

using namespace RestLink;

void listUsers(Api *api);

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.addLibraryPath("../plugins");

    QLoggingCategory::setFilterRules("restlink.info = true");

    PluginManager::enableDiscovery();

    Api api;
    api.setUrl(QUrl("sqlite:///home/commander/Projects/SODEC/SodecSureApi/SodecSure.sqlite"));

    UserController *controller = new UserController();

    Request request("/register-controller");
    request.setController(controller);

    api.post(request, Body(), [](Response *response) {
        if (response->isSuccess()) {
            qDebug() << response->readBody();
        } else {
            qDebug() << "Handler registration failed";
        }
    });

    listUsers(&api);

    return app.exec();
}

void listUsers(Api *api)
{
    auto handleResponse = [](Response *response) {
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

        static int count = 0;
        if (++count == 2)
            qApp->quit();
    };

    //api->post("/query", "SELECT * FROM garages", handleResponse);

    //api->get("/tables", handleResponse);
    api->get("/app/users/1", handleResponse);
    api->get("/app/users/1", handleResponse);
}
