#include "modeltest.h"

TEST_F(ModelTest, getTest)
{
    ASSERT_TRUE(model.get(1));

    ASSERT_EQ(log.count(), 1);
    ASSERT_EQ(log.at(0).toStdString(), R"(SELECT * FROM "Products" WHERE "id" = 1 LIMIT 1)");

    const QJsonObject product = model.jsonObject();

    const int id = product.value("id").toInt();
    ASSERT_EQ(id, 1);

    const QString name = product.value("name").toString();
    ASSERT_EQ(name.toStdString(), "Apple");

    const QString description = product.value("description").toString();
    ASSERT_EQ(description.toStdString(), "Fresh red apple");

    const double price = product.value("price").toDouble();
    ASSERT_EQ(price, 0.5);

    const QString barcode = product.value("barcode").toString();
    ASSERT_EQ(barcode.toStdString(), "1234567890123");

    // Marked as hidden
    ASSERT_FALSE(product.contains("category_id"));
}
