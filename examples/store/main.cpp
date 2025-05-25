#include <QtCore/qdir.h>

#include <QtGui/qguiapplication.h>

#include <QtQml/qqmlapplicationengine.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("Store");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("RestLink");
    app.addLibraryPath(app.applicationDirPath() + "/../plugins");

    QDir::setCurrent(TEST_DATA_DIR);

    QQmlApplicationEngine engine;
    engine.addImportPath(app.applicationDirPath() + "/../qml");
    engine.loadFromModule("Store", "App");

    return app.exec();
}
