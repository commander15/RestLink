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

    QLoggingCategory::setFilterRules("restlink.info = true");

    PluginManager::enableDiscovery();

    Api api;
    api.setUrl(QUrl("sqlite:///home/commander/Projects/SODEC/SodecSureApi/SodecSure.sqlite"));

    UserController *controller = new UserController();

    Request request("/restlink/register-controller");
    request.setController(controller);

    api.get(request, [&api](Response *response) {
        if (response->isSuccess()) {
            qDebug() << "Handler registered";
            listUsers(&api);
        } else {
            qDebug() << "Handler registration failed";
        }
    });

    return app.exec();
}

void listUsers(Api *api)
{
    auto handleResponse = [](Response *response) {
        if (!response->isSuccess()) {
            qDebug() << response->httpReasonPhrase();
            qDebug() << response->networkErrorString();
            return;
        }

        qDebug() << "HTTP " << response->httpStatusCode();
        const QByteArrayList headers = response->headerList();
        for (const QByteArray &header : headers)
            qDebug() << header << ": " << response->header(header);
        qDebug() << '\n';
        qDebug().noquote() << response->readBody();

        static int count = 0;
        ++count;

        if (count == 1)
            qApp->quit();
    };

    //api->post("/query", "SELECT * FROM garages", handleResponse);

    api->get("/tables", handleResponse);
    //api->get("/users/1", handleResponse);
    //api->get("/app/users/1", handleResponse);
}
