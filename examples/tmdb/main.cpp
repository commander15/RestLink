#include <QtGui/qguiapplication.h>

#include <QtQml/qqmlapplicationengine.h>
#include <QtQml/qqmlcontext.h>
#include <qloggingcategory.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QLoggingCategory::setFilterRules("restlink.info=true");

    const QUrl url("qrc:/qt/qml/TmdbApp/main.qml");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("TMDB_API_KEY", qEnvironmentVariable("TMDB_API_KEY"));
    engine.addImportPath(app.applicationDirPath() + "/../qml");
    engine.load(url);

    return app.exec();
}
