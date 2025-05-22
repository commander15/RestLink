#include "sqltest.h"

#include <QtCore/qfile.h>

#include <utils/querybuilder.h>
#include <utils/queryrunner.h>
#include <utils/jsonutils.h>

SqlTest::SqlTest(int configIndex)
{
    SqlLog::disableLogging();
    api = createApi(configIndex);
    SqlLog::enableLogging();
}

SqlTest::~SqlTest()
{
    delete api;
}

void SqlTest::SetUp()
{
}

void SqlTest::TearDown()
{
    api->closeDatabase();
}

Api *SqlTest::createApi(int configIndex)
{
    static int index = 0;
    const QUrl url(QStringLiteral(DB_URL).arg(index++));
    Api *api = Api::api(url);

    static QStringList databaseCreationScript;
    if (databaseCreationScript.isEmpty()) {
        const QStringList files = { "structure.sql", "data.sql" };

        for (const QString &fileName : files) {
            QFile script(QStringLiteral(DATA_DIR) + "/store/" + fileName);
            if (!script.open(QIODevice::ReadOnly|QIODevice::Text)) {
                qFatal() << "Can't open database script file " << fileName;
                continue;
            }

            const QStringList statements = QueryBuilder::statementsFromScript(script.readAll());
            if (!statements.isEmpty())
                databaseCreationScript.append(statements);
        }
    }

    for (const QString &statement : databaseCreationScript) {
        bool success = false;
        QueryRunner::exec(statement, api, &success);

        if (!success) {
            qFatal() << "Can't execute SQL statement: " << statement;
            continue;
        }
    }

    static QHash<int, QJsonObject> configurations;
    if (!configurations.contains(configIndex)) {
        const QString fileName = QStringLiteral(DATA_DIR) + "/store/configuration%1.json";
        QFile configurationFile(fileName.arg(configIndex >= 0 ? '_' + QString::number(configIndex) : ""));
        if (configurationFile.open(QIODevice::ReadOnly))
            configurations.insert(configIndex, JsonUtils::objectFromRawData(configurationFile.readAll()));
    }

    api->configure(configurations.value(configIndex));
    return api;
}
