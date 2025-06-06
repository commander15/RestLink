#include "queryrunnertest.h"

#include <utils/queryrunner.h>

#include <QtCore/qjsonarray.h>

using namespace RestLink;
using namespace RestLink::Sql;

TEST_F(QueryRunnerTest, ReturnsDataObjectWhenArrayIsFalse)
{
    Query query;
    query.statement = R"(SELECT * FROM Products WHERE name="Apple")";
    query.array = false; // Want object instead of array

    const QJsonObject object = QueryRunner::exec(query, api);

    const QJsonObject data = object.value("data").toObject();

    const int id = data.value("id").toInt();
    EXPECT_EQ(id, 1);

    const QString name = data.value("name").toString();
    EXPECT_EQ(name.toStdString(), "Apple");

    const QString description = data.value("description").toString();
    EXPECT_EQ(description.toStdString(), "Fresh red apple");

    const double price = data.value("price").toDouble();
    EXPECT_EQ(price, 0.5);

    const QString barcode = data.value("barcode").toString();
    EXPECT_EQ(barcode.toStdString(), "1234567890123");
}

TEST_F(QueryRunnerTest, ReturnsDataArrayWhenArrayIsTrue)
{
    Query query;
    query.statement = R"(SELECT * FROM Products WHERE name="Apple")";
    query.array = true; // Want array instead of object

    const QJsonObject object = QueryRunner::exec(query, api);

    const QJsonArray dataArray = object.value("data").toArray();
    const QJsonObject data = dataArray.first().toObject();

    const int id = data.value("id").toInt();
    EXPECT_EQ(id, 1);

    const QString name = data.value("name").toString();
    EXPECT_EQ(name.toStdString(), "Apple");

    const QString description = data.value("description").toString();
    EXPECT_EQ(description.toStdString(), "Fresh red apple");

    const double price = data.value("price").toDouble();
    EXPECT_EQ(price, 0.5);

    const QString barcode = data.value("barcode").toString();
    EXPECT_EQ(barcode.toStdString(), "1234567890123");
}
