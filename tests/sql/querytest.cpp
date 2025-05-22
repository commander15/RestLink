#include "querytest.h"

#include <utils/queryrunner.h>

#include <QtCore/qjsonarray.h>

using namespace RestLink;
using namespace RestLink::Sql;

TEST_F(QueryRunnerTest, queryRunnerReturnDataObject)
{
    Query query;
    query.statement = R"(SELECT * FROM Products WHERE name="Apple")";
    query.array = false; // Want object instead of array

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

TEST_F(QueryRunnerTest, queryRunnerReturnDataArray)
{
    Query query;
    query.statement = R"(SELECT * FROM Products WHERE name="Apple")";
    query.array = true; // Want array instead of object

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
