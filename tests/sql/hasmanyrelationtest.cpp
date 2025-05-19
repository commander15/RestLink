#include "hasmanyrelationtest.h"

#include <QtCore/qjsonarray.h>

TEST_F(HasManyRelationTest, getTest)
{
    initializeTest();

    ASSERT_TRUE(root.load({ "items" }));

    const QJsonObject sale = root.jsonObject();

    const QJsonArray items = sale.value("items").toArray();
    ASSERT_EQ(items.count(), 2);

    QJsonObject item;

    item = items.at(0).toObject();
    ASSERT_EQ(item.value("id").toInt(), 1);
    ASSERT_EQ(item.value("quantity").toInt(), 2);

    item = items.at(1).toObject();
    ASSERT_EQ(item.value("id").toInt(), 2);
    ASSERT_EQ(item.value("quantity").toInt(), 1);

    ASSERT_EQ(log.count(), 2);

    QString saleQuery = R"(SELECT * FROM "Sales" WHERE "id" = 1 LIMIT 1)";
    ASSERT_EQ(log.at(0).toStdString(), saleQuery.toStdString());

    QString itemsQuery = R"(SELECT * FROM "SaleItems" WHERE "sale_id" = 1)";
    ASSERT_EQ(log.at(1).toStdString(), itemsQuery.toStdString());
}

TEST_F(HasManyRelationTest, updateTest)
{
    ASSERT_TRUE(root.load({ "items" }));

    QJsonObject sale = root.jsonObject();

    QJsonArray items = sale.value("items").toArray();

    QJsonObject item = items.at(0).toObject();
    items.replace(0, item);

    sale.insert("items", items);

    ASSERT_TRUE(root.update());
}

TEST_F(HasManyRelationTest, insertTest)
{
    ASSERT_TRUE(false);
}

TEST_F(HasManyRelationTest, deleteTest)
{
    ASSERT_TRUE(root.load({ "items" }));
    ASSERT_TRUE(root.deleteData());

    qDebug() << log;
    ASSERT_EQ(log.count(), 5);

    QString itemsQuery = R"(DELETE FROM "SaleItems" WHERE "id" = %1)";
    ASSERT_EQ(log.at(2).toStdString(), itemsQuery.arg(1).toStdString());
    ASSERT_EQ(log.at(3).toStdString(), itemsQuery.arg(2).toStdString());

    QString saleQuery = R"(DELETE FROM "Sales" WHERE "id" = 1)";
    ASSERT_EQ(log.at(4).toStdString(), saleQuery.toStdString());
}


