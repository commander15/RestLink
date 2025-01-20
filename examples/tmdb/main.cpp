#include <QtGui/qguiapplication.h>

#include <QtQml/qqmlapplicationengine.h>
#include <QtQml/qqmlcontext.h>
#include <qloggingcategory.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QLoggingCategory::setFilterRules("restlink.info=true");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("TMDB_API_KEY", qEnvironmentVariable("TMDB_API_KEY"));
    engine.addImportPath(app.applicationDirPath() + "/../qml");
    engine.loadFromModule("TmdbApp", "Main");

    return app.exec();
}
