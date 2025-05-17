#include "belongstorelationtest.h"

TEST_F(BelongsToRelationTest, getTest)
{
    initializeTest();

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

TEST_F(BelongsToRelationTest, insertTest)
{
    QJsonObject stock;
    stock.insert("quantity", 50);

    QJsonObject product;
    product.insert("name", "Orange");
    product.insert("description", "juicy sugar powered fruit !");
    product.insert("price", 5.5);
    product.insert("category_id", 1);
    stock.insert("product", product);

    root = Model("stocks", api);
    root.fill(stock);
    ASSERT_TRUE(root.insert());

    const QVariant id = root.primary().toInt();
    root = Model("stocks", api);
    ASSERT_TRUE(root.get(id));
    ASSERT_TRUE(root.load({ "product" }));

    ASSERT_EQ(log.count(), 5);

    QString productQuery = R"(INSERT INTO "Products" ("name", "description", "price", "created_at") VALUES ('Orange', 'juicy sugar powered fruit !', 5.5, '%1'))";
    ASSERT_EQ(log.at(1).toStdString(), productQuery.arg(creationTimestamp("product")).toStdString());

    QString stockQuery = R"(INSERT INTO "Stocks" ("quantity", "product_id", "created_at") VALUES (50, 4, '%1'))";
    ASSERT_EQ(log.at(2).toStdString(), stockQuery.arg(creationTimestamp()).toStdString());
}

TEST_F(BelongsToRelationTest, updateTest)
{
    ASSERT_TRUE(root.load({ "product" }));
    QJsonObject stock = root.jsonObject();

    QJsonObject product = stock.value("product").toObject();
    product.insert("name", "Apple Pie");
    product.insert("description", "Original apple");
    stock.insert("product", product);

    root.fill(stock);
    ASSERT_TRUE(root.update());

    ASSERT_EQ(log.count(), 4);

    QString productQuery = R"(UPDATE "Products" SET "name" = 'Apple Pie', "description" = 'Original apple', "price" = 0.5, "barcode" = '1234567890123', "category_id" = 1, "created_at" = '%1', "updated_at" = '%2' WHERE "id" = 1)";
    ASSERT_EQ(log.at(2).toStdString(), productQuery.arg(creationTimestamp("product"), updateTimestamp("product")).toStdString());

    QString stockQuery = R"(UPDATE "Stocks" SET "quantity" = 100, "product_id" = 1, "created_at" = '%1', "updated_at" = '%2' WHERE "id" = 1)";
    ASSERT_EQ(log.at(3).toStdString(), stockQuery.arg(creationTimestamp(), updateTimestamp()).toStdString());
}

TEST_F(BelongsToRelationTest, deleteTest)
{
    ASSERT_TRUE(root.load({ "product" }));
    ASSERT_TRUE(root.deleteData());

    ASSERT_EQ(log.count(), 5);
    ASSERT_EQ(log.at(2).toStdString(), R"(DELETE FROM "Products" WHERE "id" = 1)");
    ASSERT_EQ(log.at(3).toStdString(), R"(DELETE FROM "Stocks" WHERE "id" = 1)");
}
