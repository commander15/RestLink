#include "sqllog.h"
#include "sqltest.h"

#include <gtest/gtest.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qfile.h>
#include <QtCore/qjsonarray.h>

#include <api.h>
#include <utils/jsonutils.h>

using namespace RestLink::Sql;

void generateConfigurationFile();

void init(QCoreApplication &)
{
    static const char *categoryName = "restlink.sql";

    QLoggingCategory::setFilterRules(QStringLiteral("%1.info=true\n%1.warning=true").arg(categoryName));

    static QtMessageHandler defaultMessageHandler = nullptr;

    defaultMessageHandler = qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        if (strcmp(context.category, categoryName) == 0) {
            if (type == QtMsgType::QtInfoMsg)
                SqlLog::log(msg);
            else if (type == QtWarningMsg)
                SqlLog::logError(msg);
        } else
            defaultMessageHandler(type, context, msg);
    });

    generateConfigurationFile();
}

void cleanup(QCoreApplication &)
{
    // Even in tests, we need to release memory
    Api::cleanupApis();
}

QJsonObject mergeObjects(const QJsonObject &c1, const QJsonObject &c2)
{
    QJsonObject result = c1;  // Start with all from c1

    for (const QString &key : c2.keys()) {
        const QJsonValue v2 = c2.value(key);

        if (!result.contains(key)) {
            // If c1 doesn't have this key, just take from c2
            result.insert(key, v2);
            continue;
        }

        const QJsonValue v1 = result.value(key);

        if (v1.isObject() && v2.isObject()) {
            // Recursively merge nested objects
            result.insert(key, mergeObjects(v1.toObject(), v2.toObject()));
        } else if (v1.isArray() && v2.isArray()) {
            // Merge arrays without duplicates
            QJsonArray mergedArray = v1.toArray();
            QSet<QJsonValue> seen;

            for (const QJsonValue &val : mergedArray)
                seen.insert(val);

            for (const QJsonValue &val : v2.toArray()) {
                if (!seen.contains(val)) {
                    mergedArray.append(val);
                    seen.insert(val);
                }
            }

            result.insert(key, mergedArray);
        } else {
            // For primitives or mismatched types, overwrite with c2
            result.insert(key, v2);
        }
    }

    return result;
}

void generateConfigurationFile()
{
    static const auto readConfigObject = [](int index) {
        QFile file(QStringLiteral(DATA_DIR) + "/store/configuration_" + QString::number(index) + ".json");
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Failed to open file:" << file.fileName();
            return QJsonObject();
        }
        return JsonUtils::objectFromRawData(file.readAll());
    };

    QJsonObject configuration;
    auto loadConfig = [&configuration](int index) {
        const QJsonObject object = readConfigObject(index);
        SqlTest::s_configurations.insert(index, object);
        configuration = mergeObjects(configuration, object);
    };

    for (int i(0); i <= 5; ++i)
        loadConfig(i);

    SqlTest::s_configurations.insert(-1, configuration);

    QFile file(QStringLiteral(DATA_DIR) + "/store/configuration.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(configuration).toJson(QJsonDocument::Indented));
        file.flush();
        file.close();
    }
}
