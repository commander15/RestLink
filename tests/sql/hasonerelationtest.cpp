#include "hasonerelationtest.h"

TEST_F(HasOneRelationTest, SuccessfulCreate)
{
    QJsonObject product;
    product.insert("name", "Paw paw");
    product.insert("description", "sugar fruit !");
    product.insert("price", 50.5);
    product.insert("barcode", "3214567890123");

    QJsonObject category;
    category.insert("id", 1);
    product.insert("category", category);

    QJsonObject stock;
    stock.insert("quantity", 200);
    product.insert("stock", stock);

    root = Model("products", api);
    root.fill(product);
    ASSERT_TRUE(root.insert());

    ASSERT_EQ(log.count(), 3);

    QString productQuery = R"(INSERT INTO "Products" ("name", "description", "price", "barcode", "category_id", "created_at") VALUES ('Paw paw', 'sugar fruit !', 50.5, '3214567890123', 1, '%1'))";
    // ASSERT_EQ(log.at(1).toStdString(), productQuery.arg(creationTimestamp()).toStdString());

    QString stockQuery = R"(INSERT INTO "Stocks" ("quantity", "product_id", "created_at") VALUES (200, 4, '%1'))";
    ASSERT_EQ(log.at(2).toStdString(), stockQuery.arg(creationTimestamp("stock")).toStdString());
}

TEST_F(HasOneRelationTest, SuccessfulRead)
{
    ASSERT_TRUE(root.load({ "stock" }));
    const QJsonObject product = root.jsonObject();

    const QJsonObject stock = product.value("stock").toObject();

    const int id = stock.value("id").toInt();
    ASSERT_EQ(id, 1);

    const int quantity = stock.value("quantity").toInt();
    ASSERT_EQ(quantity, 100);

    ASSERT_EQ(log.count(), 2);
    // ASSERT_EQ(log.at(0).toStdString(), R"(SELECT * FROM "Products" WHERE "id" = 1 LIMIT 1)");
    ASSERT_EQ(log.at(1).toStdString(), R"(SELECT * FROM "Stocks" WHERE "product_id" = 1 LIMIT 1)");
}

TEST_F(HasOneRelationTest, SuccessfulUpdate)
{
    ASSERT_TRUE(root.load({ "stock" }));
    QJsonObject product = root.jsonObject();

    QJsonObject stock = product.value("stock").toObject();
    stock.insert("quantity", 50);
    product.insert("stock", stock);

    root.fill(product);
    ASSERT_TRUE(root.update());

    // We check now

    ASSERT_EQ(log.count(), 4);

    ASSERT_EQ(log.at(1).toStdString(), R"(SELECT * FROM "Stocks" WHERE "product_id" = 1 LIMIT 1)");

    QString productQuery = R"(UPDATE "Products" SET "name" = 'Apple', "description" = 'Fresh red apple', "price" = 0.5, "barcode" = '1234567890123', "category_id" = 1, "created_at" = '%1', "updated_at" = '%2' WHERE "id" = 1)";
    ASSERT_EQ(log.at(2).toStdString(), productQuery.arg(creationTimestamp(), updateTimestamp()).toStdString());

    QString stockQuery = R"(UPDATE "Stocks" SET "quantity" = 50, "product_id" = 1, "created_at" = '%1', "updated_at" = '%2' WHERE "id" = 1)";
    ASSERT_EQ(log.at(3).toStdString(), stockQuery.arg(creationTimestamp("stock"), updateTimestamp("stock")).toStdString());

    ASSERT_TRUE(root.load({ "stock" }));
    product = root.jsonObject();
    stock = product.value("stock").toObject();

    const int id = stock.value("id").toInt();
    ASSERT_EQ(id, 1);

    const int quantity = stock.value("quantity").toInt();
    ASSERT_EQ(quantity, 50);
}

TEST_F(HasOneRelationTest, SuccessfulDelete)
{
    ASSERT_TRUE(root.load({ "stock" }));
    ASSERT_TRUE(root.deleteData());

    ASSERT_EQ(log.count(), 4);
    ASSERT_EQ(log.at(2).toStdString(), R"(DELETE FROM "Products" WHERE "id" = 1)");
    ASSERT_EQ(log.at(3).toStdString(), R"(DELETE FROM "Stocks" WHERE "id" = 1)");
}
