#include "belongstoonerelationtest.h"

TEST_F(BelongsToOneRelationTest, getTest)
{
    ASSERT_TRUE(root.load({ "product" }));
    const QJsonObject stock = root.jsonObject();

    const QJsonObject product = stock.value("product").toObject();

    const int id = product.value("id").toInt();
    ASSERT_EQ(id, 1);

    const QString name = product.value("name").toString();
    ASSERT_EQ(name.toStdString(), "Apple");

    const QString description = product.value("description").toString();
    ASSERT_EQ(description.toStdString(), "Fresh red apple");

    ASSERT_EQ(log.count(), 2);
    ASSERT_EQ(log.at(0).toStdString(), R"(SELECT * FROM "Stocks" WHERE "id" = 1 LIMIT 1)");
    ASSERT_EQ(log.at(1).toStdString(), R"(SELECT * FROM "Products" WHERE "id" = 1 LIMIT 1)");
}

TEST_F(BelongsToOneRelationTest, insertTest)
{
    QJsonObject stock;
    stock.insert("quantity", 50);

    QJsonObject product;
    product.insert("id", 1);
    stock.insert("product", product);

    root = Model("stocks", api);
    root.fill(stock);
    ASSERT_TRUE(root.insert());

    ASSERT_EQ(log.count(), 2);

    QString stockQuery = R"(INSERT INTO "Stocks" ("quantity", "product_id", "created_at") VALUES (50, 1, '%1'))";
    ASSERT_EQ(log.at(1).toStdString(), stockQuery.arg(creationTimestamp()).toStdString());
}

TEST_F(BelongsToOneRelationTest, updateTest)
{
    ASSERT_TRUE(root.load({ "product" }));
    QJsonObject stock = root.jsonObject();

    QJsonObject product = stock.value("product").toObject();
    product.insert("id", 2);
    stock.insert("product", product);

    root.fill(stock);
    ASSERT_TRUE(root.update());

    ASSERT_EQ(log.count(), 3);

    QString stockQuery = R"(UPDATE "Stocks" SET "quantity" = 100, "product_id" = 2, "created_at" = '%1', "updated_at" = '%2' WHERE "id" = 1)";
    ASSERT_EQ(log.at(2).toStdString(), stockQuery.arg(creationTimestamp(), updateTimestamp()).toStdString());
}

TEST_F(BelongsToOneRelationTest, deleteTest)
{
    ASSERT_TRUE(root.load({ "product" }));
    ASSERT_TRUE(root.deleteData());

    ASSERT_EQ(log.count(), 3);
    ASSERT_EQ(log.at(2).toStdString(), R"(DELETE FROM "Stocks" WHERE "id" = 1)");
}
