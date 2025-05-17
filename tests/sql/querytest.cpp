#include <gtest/gtest.h>

#include <api.h>
#include <qjsonarray.h>
#include <utils/queryrunner.h>

using namespace RestLink;
using namespace RestLink::Sql;

TEST(QueryTest, queryRunnerReturnDataObject)
{
    Query query;
    query.statement = R"(SELECT * FROM Products WHERE name="Apple")";
    query.array = false; // Want object instead of array

    Api *api = Api::api(QUrl(DB_URL));
    const QJsonObject object = QueryRunner::exec(query, api);

    const QJsonObject data = object.value("data").toObject();

    const QString name = data.value("name").toString();
    ASSERT_EQ(name.toStdString(), "Apple");

    const QString description = data.value("description").toString();
    ASSERT_EQ(description.toStdString(), "Fresh red apple");

    const double price = data.value("price").toDouble();
    ASSERT_EQ(price, 0.5);

    const QString barcode = data.value("barcode").toString();
    ASSERT_EQ(barcode.toStdString(), "1234567890123");
}

TEST(QueryTest, queryRunnerReturnDataArray)
{
    Query query;
    query.statement = R"(SELECT * FROM Products WHERE name="Apple")";
    query.array = true; // Want array instead of object

    Api *api = Api::api(QUrl(DB_URL));
    const QJsonObject object = QueryRunner::exec(query, api);

    const QJsonArray dataArray = object.value("data").toArray();
    const QJsonObject data = dataArray.first().toObject();

    const QString name = data.value("name").toString();
    ASSERT_EQ(name.toStdString(), "Apple");

    const QString description = data.value("description").toString();
    ASSERT_EQ(description.toStdString(), "Fresh red apple");

    const double price = data.value("price").toDouble();
    ASSERT_EQ(price, 0.5);

    const QString barcode = data.value("barcode").toString();
    ASSERT_EQ(barcode.toStdString(), "1234567890123");
}
