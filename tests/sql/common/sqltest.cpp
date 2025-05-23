#include "sqltest.h"

#include <QtCore/qfile.h>

#include <utils/querybuilder.h>
#include <utils/queryrunner.h>

QJsonObject mergeObjects(const QJsonObject &c1, const QJsonObject &c2);

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
        const QStringList files = { "structure.sql", "content.sql" };

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

    QJsonObject configuration = mergeObjects(s_configurations.value(0), s_configurations.value(configIndex));
    api->configure(configuration);
    return api;
}

QHash<int, QJsonObject> SqlTest::s_configurations;
