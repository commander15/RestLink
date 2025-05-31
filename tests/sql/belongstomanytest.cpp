#include "belongstomanytest.h"

TEST_F(BelongsToManyTest, SuccessfulCreate)
{
    QJsonObject product;
    product.insert("name", "Tomato");
    product.insert("description", "red thing");
    product.insert("price", 500.5);
    product.insert("barcode", "877565456654");

    QJsonObject category;
    category.insert("id", 2);
    product.insert("category", category);

    root = Model("products", api);
    root.fill(product);
    EXPECT_TRUE(root.insert());


    ASSERT_GE(log.count(), 2);

    QString query = R"(INSERT INTO "Products" ("name", "description", "price", "barcode", "category_id", "created_at") VALUES ('%1', '%2', 500.5, '%3', '%4'))";
    EXPECT_EQ(log.at(1).toStdString(), query.arg("Tomato", "red thing", "877565456654", root.field("created_at").toString()).toStdString());

    ASSERT_EQ(log.count(), 2);
}

TEST_F(BelongsToManyTest, SuccessfulRead)
{
    ASSERT_TRUE(false);
}

TEST_F(BelongsToManyTest, SuccessfulUpdate)
{
    ASSERT_TRUE(false);
}

TEST_F(BelongsToManyTest, SuccessfulDelete)
{
    ASSERT_TRUE(false);
}
