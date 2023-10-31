#include <QtGui/qguiapplication.h>

#include <QtQml/qqmlapplicationengine.h>

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    const QUrl url("qrc:/main.qml");

    QQmlApplicationEngine engine;
    engine.addImportPath(app.applicationDirPath() + "/../qml");
    engine.load(url);

    return app.exec();
}
