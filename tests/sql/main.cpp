#include "sqllog.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qfile.h>
#include <QtCore/qdir.h>

#include <api.h>
#include <utils/querybuilder.h>
#include <utils/queryrunner.h>
#include <utils/jsonutils.h>

#include <gtest/gtest.h>

using namespace RestLink::Sql;

void installMessageHandler();
void resetDatabase();
void runScript(const QString &fileName, Api *api);

void init(QCoreApplication &)
{
    QDir dir(DB_DIR);
    if (dir.exists())
        dir.removeRecursively();
    dir.mkpath(".");

    const QUrl url(DB_URL);
    Api *api = Api::api(url);

    if (!api || !api->database().isOpen()) {
        qFatal() << "Can't open database file !";
        ASSERT_TRUE(false);
    }

    installMessageHandler();
    resetDatabase();

    QFile configuration(QStringLiteral(DATA_DIR) + "/store/configuration.json");
    if (!configuration.open(QIODevice::ReadOnly)) {
        qFatal() << "Can't open configuration file !";
        ASSERT_TRUE(false);
    }

    const QJsonObject configurationObject = JsonUtils::objectFromRawData(configuration.readAll());
    api->configure(configurationObject);
}

void cleanup(QCoreApplication &)
{
    // Even in tests, we need to release memory
    Api::cleanupApis();
}

void installMessageHandler()
{
    static QtMessageHandler defaultMessageHandler;

    defaultMessageHandler = qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        if (strcmp(context.category, "restlink.sql") == 0 && type == QtMsgType::QtInfoMsg) {
            SqlLog::log(msg);
            if (!SqlLog::isLoggingEnabled())
                return;
        }

        defaultMessageHandler(type, context, msg);
    });
}

void resetDatabase()
{
    Api *api = Api::api(QUrl(DB_URL));
    SqlLog::disableLogging();
    runScript("store/destroy.sql", api);
    runScript("store/structure.sql", api);
    runScript("store/data.sql", api);
    SqlLog::enableLogging();
}

void runScript(const QString &fileName, Api *api)
{
    QFile script(QStringLiteral(DATA_DIR) + '/' + fileName);
    if (!script.open(QIODevice::ReadOnly|QIODevice::Text)) {
        qFatal() << "Can't open database creation script file !";
        ASSERT_TRUE(false);
    }

    const QStringList statements = QueryBuilder::statementsFromScript(script.readAll());
    for (const QString &statement : statements) {
        Query query;
        query.statement = statement;
        bool success = false;

        QueryRunner::exec(query, api, &success);
        if (!success) {
            qFatal() << "Error during database creation !";
            ASSERT_TRUE(false);
        }
    }
}
