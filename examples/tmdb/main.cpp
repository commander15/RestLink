#include <QtGui/qguiapplication.h>

#include <QtQml/qqmlapplicationengine.h>
#include <QtQml/qqmlcontext.h>
#include <qloggingcategory.h>

int main(int argc, char *argv[])
{
#ifdef Q_OS_WASM
    qputenv("QT_WASM_USE_EMSCRIPTEN_FETCH", "1");
#endif

    QGuiApplication app(argc, argv);

    QLoggingCategory::setFilterRules("restlink.info=true");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("TMDB_BEARER_TOKEN", TMDB_BEARER_TOKEN);
    engine.addImportPath(app.applicationDirPath() + "/../qml");
    engine.load(QUrl("qrc:/qml/TmdbApp/Main.qml"));

    return app.exec();
}
