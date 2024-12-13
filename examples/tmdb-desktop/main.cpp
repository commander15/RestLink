#include <QtCore/qcoreapplication.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qtimer.h>

#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>

#include <RestLink/api.h>
#include <RestLink/apirequest.h>
#include <RestLink/apireply.h>
#include <RestLink/apicache.h>

using namespace RestLink;

void runRequest(const QString &endpoint, const QList<ApiRequestParameter> &parameters, Api *api)
{
    static QDir folder("/home/commander/Desktop/" + api->apiName());
    if (!folder.exists())
        folder.mkpath(".");

    ApiRequest request;
    request.setEndpoint(endpoint);
    request.setParameters(parameters);

    QString url = request.urlPath();
    if (url.count('/') > 1)
        folder.mkpath(url.section("/", 1, -2));

    QFile *file = new QFile(folder.path() + url + ".json", api);

    api->get(request, [file, api](ApiReply *reply) {
        QTextStream out(stdout);
        out << reply->endpoint() << " => ";

        if (reply->isSuccess()) {
            const QByteArray data = reply->readBody();

            if (file->open(QIODevice::WriteOnly|QIODevice::Text)) {
                file->write(data);
                file->flush();
                file->close();
                file->deleteLater();
            }

            out << data.size() << " byte(s) downloaded" << Qt::endl;
        } else if (reply->hasNetworkError()) {
            out << "Network error " << reply->networkError() << ": " << reply->networkErrorString() << Qt::endl;
        } else if (!reply->isHttpStatusSuccess()) {
            out << "HTTP " << reply->httpStatusCode() << ' ' << reply->httpReasonPhrase() << Qt::endl;
        }

        int count = api->property("count").toInt() - 1;
        if (count == 0)
            qApp->quit();
        else
            api->setProperty("count", count);
    });
}

void run(Api *api)
{
    const QMultiHash<QString, ApiRequestParameter> endpoints = {
        { "/configuration", {} },
        { "/discover/movie", {} },
        { "/discover/tv", {} },
        {
         "/search/company",
         {
          ApiRequestParameter("query", "Marvel")
         }
        },
        {
         "/movie/{movie_id}",
         {
          ApiRequestParameter("movie_id", 335983, ApiRequestParameter::UrlPathScope)
         }
        }
    };

    const QStringList endpointNames = endpoints.keys();
    for (const QString &endpoint : endpointNames)
        QTimer::singleShot(1500, api, [endpoint, endpoints, api] {
            runRequest(endpoint, endpoints.values(endpoint), api);
        });

    api->setProperty("count", endpoints.size());
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QLoggingCategory::setFilterRules("restlink.info=true");

    QNetworkAccessManager manager;
    //manager.setTransferTimeout(10000);
    manager.setCache(new ApiCache(&manager));

    Api api;
    api.setNetworkAccessManager(&manager);
    api.configureApi(QUrl::fromLocalFile("/home/commander/Downloads/TmdbConfig.json"));
    QObject::connect(&api, &Api::configurationCompleted, &app, [&api] { run(&api); });
    QObject::connect(&api, &Api::configurationFailed, &app, &QCoreApplication::quit);

    return app.exec();
}
