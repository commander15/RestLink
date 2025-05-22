#include "hasmanyrelationtest.h"

#include <QtCore/qjsonarray.h>

TEST_F(HasManyRelationTest, getTest)
{
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

    ASSERT_EQ(log.count(), 4);

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
    items.removeLast();

    QJsonObject item;

    item = items.at(0).toObject();
    QJsonObject product;
    product.insert("id", 2);
    item.insert("product", product);
    item.insert("quantity", 4);
    items.replace(0, item);

    item = QJsonObject();
    product.insert("id", 3);
    item.insert("product", product);
    item.insert("quantity", 10);
    items.append(item);

    sale.insert("items", items);

    root.fill(sale);
    ASSERT_TRUE(root.update());

    ASSERT_EQ(log.count(), 8);

    QString saleQuery = R"(UPDATE "Sales" SET "number" = 1, "amount" = 3, "created_at" = '%1', "updated_at" = '%2' WHERE "id" = 1)";
    ASSERT_EQ(log.at(4).toStdString(), saleQuery.arg(root.field("created_at").toString(), root.field("updated_at").toString()).toStdString());

    QString itemQuery = R"(UPDATE "SaleItems" SET "quantity" = %1, "sale_id" = 1, "product_id" = 2 WHERE "id" = %2)";
    ASSERT_EQ(log.at(5).toStdString(), itemQuery.arg(4).arg(1).toStdString());

    QString insertQuery = R"(INSERT INTO "SaleItems" ("quantity", "sale_id", "product_id") VALUES (%1, 1, 3))";
    ASSERT_EQ(log.at(6).toStdString(), insertQuery.arg(10).toStdString());

    QString deleteQuery = R"(DELETE FROM "SaleItems" WHERE "id" NOT IN (%1))";
    ASSERT_EQ(log.at(7).toStdString(), deleteQuery.arg("1, 4").toStdString());
}

TEST_F(HasManyRelationTest, insertTest)
{
    root = Model(root.resourceInfo().name(), api);

    QJsonObject sale;
    sale.insert("number", 4);
    sale.insert("amount", 200.5);

    QJsonObject item;
    item.insert("quantity", 4);

    QJsonObject product;
    product.insert("id", 1);
    item.insert("product", product);

    sale.insert("items", QJsonArray() << item);

    root.fill(sale);

    ASSERT_TRUE(root.insert());

    ASSERT_EQ(log.count(), 3);

    QString saleQuery = R"(INSERT INTO "Sales" ("number", "amount", "created_at") VALUES (4, 200.5, '%1'))";
    ASSERT_EQ(log.at(1).toStdString(), saleQuery.arg(root.field("created_at").toString()).toStdString());
}

TEST_F(HasManyRelationTest, deleteTest)
{
    ASSERT_TRUE(root.load({ "items" }));
    ASSERT_TRUE(root.deleteData());

    ASSERT_EQ(log.count(), 7);

    QString saleQuery = R"(DELETE FROM "Sales" WHERE "id" = 1)";
    ASSERT_EQ(log.at(4).toStdString(), saleQuery.toStdString());

    QString itemsQuery = R"(DELETE FROM "SaleItems" WHERE "id" = %1)";
    ASSERT_EQ(log.at(5).toStdString(), itemsQuery.arg(1).toStdString());
    ASSERT_EQ(log.at(6).toStdString(), itemsQuery.arg(2).toStdString());
}


