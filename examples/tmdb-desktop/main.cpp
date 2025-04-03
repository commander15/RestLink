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
    static QDir folder(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + '/' + api->name());
    if (!folder.exists())
        folder.mkpath(".");

    QString url = request.urlPath();
    if (url.count('/') > 1)
        folder.mkpath(url.section("/", 1, -2));

    QFile *file = new QFile(folder.path() + url + ".json", api);

    api->get(request, [file, api](Response *reply) {
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
    QList<Request> requests;
    requests.append("/configuration");
    requests.append("/discover/movie");
    requests.append("/discover/tv");

    {
        Request request("/search/company");
        request.addQueryParameter("query", "Marvel");
        requests.append(request);
    }

    {
        Request request("/movie/{movie_id}");
        request.setPathParameter("movie_id", 335983);
        requests.append(request);
    }

    for (const Request &request : std::as_const(requests))
        runRequest(request, api);

    api->setProperty("count", requests.size());
}

class Interceptor : public RequestInterceptor
{
public:
    Request intercept(const Request &request, const Body &body, ApiBase::Operation operation) override
    {
        //qDebug().noquote() << HttpUtils::verbString(operation) << request.url(Request::PublicUrl).toString() << Qt::endl;
        return request;
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QLoggingCategory::setFilterRules("restlink.info=true");

    Api api;
    api.configure(QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/TmdbConfig.json"));
    QObject::connect(&api, &Api::configurationCompleted, &app, [&api] { run(&api); });
    QObject::connect(&api, &Api::configurationFailed, &app, &QCoreApplication::quit);

    Interceptor interceptor;
    api.addRequestInterceptor(&interceptor);

    return app.exec();
}
