#include "modeltest.h"

TEST_F(ModelTest, SuccessfulRead)
{
    ASSERT_TRUE(model.get(1));

    ASSERT_EQ(log.count(), 1);
    ASSERT_EQ(log.at(0).toStdString(), R"(SELECT * FROM "Products" WHERE "id" = 1 LIMIT 1)");

    const QJsonObject product = model.jsonObject();

    const int id = product.value("id").toInt();
    EXPECT_EQ(id, 1);

    const QString name = product.value("name").toString();
    EXPECT_EQ(name.toStdString(), "Apple");

    const QString description = product.value("description").toString();
    EXPECT_EQ(description.toStdString(), "Fresh red apple");

    const double price = product.value("price").toDouble();
    EXPECT_EQ(price, 0.5);

    const QString barcode = product.value("barcode").toString();
    EXPECT_EQ(barcode.toStdString(), "1234567890123");

    // Marked as hidden
    EXPECT_FALSE(product.contains("category_id"));
}
