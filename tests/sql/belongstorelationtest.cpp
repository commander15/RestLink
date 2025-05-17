#include "belongstorelationtest.h"

TEST_F(BelongsToRelationTest, getTest)
{
    initializeTest();

    ASSERT_TRUE(root.load({ "category" }));
    const QJsonObject product = root.jsonObject();

    const QJsonObject category = product.value("category").toObject();

    const int id = category.value("id").toInt();
    ASSERT_EQ(id, 1);

    const QString name = category.value("name").toString();
    ASSERT_EQ(name.toStdString(), "Fruits");

    const QString description = category.value("description").toString();
    ASSERT_EQ(description.toStdString(), "Natural meals");

    ASSERT_EQ(log.count(), 2);
    ASSERT_EQ(log.at(0).toStdString(), R"(SELECT * FROM "Products" WHERE "id" = 1 LIMIT 1)");
    ASSERT_EQ(log.at(1).toStdString(), R"(SELECT * FROM "Categories" WHERE "id" = 1 LIMIT 1)");
}

TEST_F(BelongsToRelationTest, insertTest)
{
    ASSERT_TRUE(false);
}

TEST_F(BelongsToRelationTest, updateTest)
{
    ASSERT_TRUE(root.load({ "category" }));
    QJsonObject product = root.jsonObject();

    QJsonObject category;
    category.insert("id", 2); // Enough for update since category is not owned
    product.insert("category", category);

    root.fill(product);
    ASSERT_TRUE(root.update());

    ASSERT_EQ(log.count(), 3);

    QString productQuery = R"(UPDATE Products SET "name" = 'Apple', "description" = 'Fresh red apple', "price" = 0.5, "barcode" = '1234567890123', "category_id" = 2, "created_at" = '%1', "updated_at" = '%2' WHERE "id" = 1)";
    ASSERT_EQ(log.at(2).toStdString(), productQuery.arg(creationTimestamp(), updateTimestamp()).toStdString());
}

TEST_F(BelongsToRelationTest, deleteTest)
{
    ASSERT_TRUE(root.load({ "category" }));
    QJsonObject product = root.jsonObject();

    ASSERT_TRUE(root.deleteData());

    ASSERT_EQ(log.count(), 3);
    ASSERT_EQ(log.at(2).toStdString(), R"(DELETE FROM "Products" WHERE "id" = 1)");
}
