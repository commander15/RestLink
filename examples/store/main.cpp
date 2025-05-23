#include <QtGui/qguiapplication.h>

#include <QtQml/qqmlapplicationengine.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("Store");
    app.setOrganizationName("RestLink");
    app.addLibraryPath(app.applicationDirPath() + "/../plugins");

    QQmlApplicationEngine engine;
    engine.addImportPath(app.applicationDirPath() + "/../qml");
    engine.loadFromModule("Store", "App");

    return app.exec();
}
