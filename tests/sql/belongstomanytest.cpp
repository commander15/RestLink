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
