#include <QtCore/qcoreapplication.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qtimer.h>
#include <QtCore/qstandardpaths.h>

#include <RestLink/restlink.h>

using namespace RestLink;

void runRequest(const Request &request, Api *api)
{
    // We will save all responses on your Desktop folder, TMDB sub folder

    static QDir folder(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + '/' + api->name());
    if (!folder.exists())
        folder.mkpath(".");

    QString url = request.urlPath();
    if (url.count('/') > 1)
        folder.mkpath(url.section("/", 1, -2));

    QFile *file = new QFile(folder.path() + url + ".json", api);

    // Pretty assync handling alert !
    api->get(request, [file, api](Response *reply) {
        // Here application output will tell you all what is happening
        // (errors included)

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

            // Download completed !
            out << data.size() << " byte(s) downloaded" << Qt::endl;
        } else if (!reply->hasHttpStatusCode()) {
            // Logging this HTTP error
            out << "HTTP " << reply->httpStatusCode() << ' ' << reply->httpReasonPhrase() << Qt::endl;
        } else if (reply->hasNetworkError()) {
            // Logging this network error
            out << "Network error " << reply->networkError() << ": " << reply->networkErrorString() << Qt::endl;
        } else {
            // Logging this ... wait, we don't known what happened < 1% of time ;)
            out << "Oops ! - something weired happened" << Qt::endl;
        }

        // Let's count before quit !
        // 3, 2, 1, ... bye bye !
        int count = api->property("count").toInt() - 1;
        if (count == 0)
            qApp->quit();
        else
            api->setProperty("count", count);
    });
}

void run(Api *api)
{
    // We prepare some requests in a list

    QList<Request> requests;
    requests.append("/configuration"); // geeky things
    requests.append("/discover/movie"); // movie discovery
    requests.append("/discover/tv"); // tv discovery

    {
        Request request("/search/company");

        // We are seaching Marvel Studios, let's go from Marvel to have more results
        request.addQueryParameter("query", "Marvel");

        requests.append(request);
    }

    {
        Request request("/movie/{movie_id}");

        // Do you like Marvel Venom movie ?
        request.setPathParameter("movie_id", 335983);

        requests.append(request);
    }

    // We run them all
    for (const Request &request : std::as_const(requests))
        runRequest(request, api);

    api->setProperty("count", requests.size());
}

class Interceptor : public AbstractRequestInterceptor
{
public:
    void intercept(AbstractRequestHandler::Method method, Request &request, Body &body) override
    {
        // we log the method and public url, no secrets inside !
        static QTextStream out(stdout);
        out << HttpUtils::verbString(method) << ' '
            << request.url(Request::PublicUrl).toString(QUrl::DecodeReserved)
            << Qt::endl;

        Q_UNUSED(body)

        // To distinguish requests by api
        Api *api = request.api();
        Q_UNUSED(api)
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("TMDB");
    app.setApplicationVersion(RESTLINK_VERSION_STR);
    app.setOrganizationName("RestLink");

    // We will reach the web with this
    NetworkManager manager;
    manager.setTransferTimeout(3000);

    // We will reduce network usage with this
    Cache cache;
    cache.setMaxCacheSize(2 * 1024 * 1024); // 2MB
    manager.setCache(&cache);

    // Hungry, let's bake some cookies
    CookieJar cookies;
    manager.setCookieJar(&cookies);

    // We will log with this
    Interceptor interceptor;
    manager.addRequestInterceptor(&interceptor);

    Api api;
    api.setName("TMDB"); // Optional
    api.setVersion(QVersionNumber(3)); // Optional
    api.setUrl(QUrl("http://api.themoviedb.org/3")); // Mandatory
    api.setBearerToken(TMDB_BEARER_TOKEN); // Mandatory if you want to Avoid HTTP - Forbidden :)
    api.setNetworkManager(&manager);

    // Let's go !
    run(&api);

    return app.exec();
}
