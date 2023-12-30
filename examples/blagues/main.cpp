#include <QtGui/qguiapplication.h>

#include <QtQml/qqmlapplicationengine.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("Blagues");
    app.setApplicationVersion("2.0");
    app.setOrganizationName("Commander Systems");

    const QUrl url("qrc:/Blagues/main.qml");

    QQmlApplicationEngine engine;
#ifdef Q_OS_LINUX
    engine.addImportPath("/opt/Commander/gcc_64/qml");
#endif
    engine.load(url);

    return app.exec();
}
