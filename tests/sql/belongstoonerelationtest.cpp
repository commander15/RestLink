#include "belongstoonerelationtest.h"

TEST_F(BelongsToOneRelationTest, SuccessfulCreate)
{
    QJsonObject product;
    product.insert("name", "Banana");
    product.insert("description", "Yellow fruit");
    product.insert("price", 40.3);
    product.insert("barcode", "42242452354");

    QJsonObject category;
    category.insert("id", 2);
    product.insert("category", category);

    root = Model("products", api);
    root.fill(product);
    EXPECT_TRUE(root.insert());

    ASSERT_GE(log.count(), 2);

    QString productQuery = R"(INSERT INTO "Products" ("name", "description", "price", "barcode", "category_id", "created_at") VALUES ('Banana', 'Yellow fruit', 40.3, '42242452354', 2, '%1'))";
    EXPECT_EQ(log.at(1).toStdString(), productQuery.arg(creationTimestamp()).toStdString());

    EXPECT_EQ(log.count(), 2);
}

TEST_F(BelongsToOneRelationTest, SuccessfulRead)
{
    EXPECT_TRUE(root.load({ "category" }));

    const QJsonObject product = root.jsonObject();
    EXPECT_EQ(product.value("id").toInt(), 1);
    EXPECT_EQ(product.value("name").toString().toStdString(), "Apple");
    EXPECT_EQ(product.value("description").toString().toStdString(), "Fresh red apple");

    const QJsonObject category = product.value("category").toObject();
    EXPECT_EQ(category.value("id").toInt(), 1);
    EXPECT_EQ(category.value("name").toString().toStdString(), "Fruits");

    ASSERT_GE(log.count(), 2);
    EXPECT_EQ(log.at(0).toStdString(), R"(SELECT * FROM "Products" WHERE "id" = 1 LIMIT 1)");
    EXPECT_EQ(log.at(1).toStdString(), R"(SELECT * FROM "Categories" WHERE "id" = 1 LIMIT 1)");
    EXPECT_EQ(log.count(), 2);
}

TEST_F(BelongsToOneRelationTest, SuccessfulUpdate)
{
    EXPECT_TRUE(root.load({ "category" }));

    QJsonObject stock = root.jsonObject();
    stock.insert("quantity", 19);

    QJsonObject category = stock.value("category").toObject();
    category.insert("id", 2);
    stock.insert("category", category);

    root.fill(stock);
    EXPECT_TRUE(root.update());

    ASSERT_GE(log.count(), 3);

    QString productQuery = R"(UPDATE "Products" SET "name" = 'Apple', "description" = 'Fresh red apple', "price" = 0.5, "barcode" = '1234567890123', "category_id" = 2, "created_at" = '%1', "updated_at" = '%2' WHERE "id" = 1)";
    EXPECT_EQ(log.at(2).toStdString(), productQuery.arg(creationTimestamp(), updateTimestamp()).toStdString());

    EXPECT_EQ(log.count(), 3);
}

TEST_F(BelongsToOneRelationTest, SuccessfulDelete)
{
    EXPECT_TRUE(root.load({ "category" }));
    EXPECT_TRUE(root.deleteData());

    ASSERT_GE(log.count(), 3);
    EXPECT_EQ(log.at(2).toStdString(), R"(DELETE FROM "Products" WHERE "id" = 1)");
    EXPECT_EQ(log.count(), 3);
}
