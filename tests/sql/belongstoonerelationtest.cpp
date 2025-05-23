#include "belongstoonerelationtest.h"

TEST_F(BelongsToOneRelationTest, getTest)
{
    ASSERT_TRUE(root.load({ "category" }));

    const QJsonObject product = root.jsonObject();
    ASSERT_EQ(product.value("id").toInt(), 1);
    ASSERT_EQ(product.value("name").toString().toStdString(), "Apple");
    ASSERT_EQ(product.value("description").toString().toStdString(), "Fresh red apple");

    const QJsonObject category = product.value("category").toObject();
    ASSERT_EQ(category.value("id").toInt(), 1);
    ASSERT_EQ(category.value("name").toString().toStdString(), "Fruits");

    ASSERT_EQ(log.count(), 2);
    ASSERT_EQ(log.at(0).toStdString(), R"(SELECT * FROM "Products" WHERE "id" = 1 LIMIT 1)");
    ASSERT_EQ(log.at(1).toStdString(), R"(SELECT * FROM "Categories" WHERE "id" = 1 LIMIT 1)");
}

TEST_F(BelongsToOneRelationTest, insertTest)
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
    ASSERT_TRUE(root.insert());

    ASSERT_EQ(log.count(), 2);

    QString productQuery = R"(INSERT INTO "Products" ("name", "description", "price", "barcode", "category_id", "created_at") VALUES ('Banana', 'Yellow fruit', 40.3, '42242452354', 2, '%1'))";
    ASSERT_EQ(log.at(1).toStdString(), productQuery.arg(creationTimestamp()).toStdString());
}

TEST_F(BelongsToOneRelationTest, updateTest)
{
    ASSERT_TRUE(root.load({ "category" }));

    QJsonObject stock = root.jsonObject();
    stock.insert("quantity", 19);

    QJsonObject category = stock.value("category").toObject();
    category.insert("id", 2);
    stock.insert("category", category);

    root.fill(stock);
    ASSERT_TRUE(root.update());

    ASSERT_EQ(log.count(), 3);

    QString productQuery = R"(UPDATE "Products" SET "name" = 'Apple', "description" = 'Fresh red apple', "price" = 0.5, "barcode" = '1234567890123', "category_id" = 2, "created_at" = '%1', "updated_at" = '%2' WHERE "id" = 1)";
    ASSERT_EQ(log.at(2).toStdString(), productQuery.arg(creationTimestamp(), updateTimestamp()).toStdString());
}

TEST_F(BelongsToOneRelationTest, deleteTest)
{
    ASSERT_TRUE(root.load({ "category" }));
    ASSERT_TRUE(root.deleteData());

    ASSERT_EQ(log.count(), 3);
    ASSERT_EQ(log.at(2).toStdString(), R"(DELETE FROM "Products" WHERE "id" = 1)");
}
