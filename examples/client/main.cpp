#include <QtGui/qguiapplication.h>

#include <QtQml/qqmlapplicationengine.h>

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    app.setApplicationName("Blagues");
    app.setApplicationVersion("2.0");
    app.setOrganizationName("Commander Systems");

    const QUrl url("qrc:/qml/main.qml");

    QQmlApplicationEngine engine;
    engine.addImportPath(app.applicationDirPath() + "/../qml");
    engine.load(url);

    return app.exec();
}
