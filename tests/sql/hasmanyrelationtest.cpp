#include "hasmanyrelationtest.h"

#include <QtCore/qjsonarray.h>

TEST_F(HasManyRelationTest, SuccessfulCreate)
{
    root = Model(root.resourceInfo().name(), api);

    QJsonObject category;
    category.insert("name", "Juice");
    category.insert("description", "Sugar liquids");

    QJsonArray products;
    QJsonObject product;

    product.insert("name", "Top Orange");
    product.insert("description", "Orange savour - artificial thing");
    product.insert("price", 19.90);
    product.insert("barcode", "335453576345");
    products.append(product);

    product.insert("name", "Top Grenadine");
    product.insert("description", "Strange red savour - artificial thing");
    product.insert("price", 39.90);
    product.insert("barcode", "587463576345");
    products.append(product);

    category.insert("products", products);

    root.fill(category);
    EXPECT_TRUE(root.insert());

    const QList<Model> models = root.relation("products").models();
    EXPECT_EQ(models.count(), 2);

    QString saleQuery = R"(INSERT INTO "Categories" ("name", "description", "created_at") VALUES ('Juice', 'Sugar liquids', '%1'))";
    QString productQuery = R"(INSERT INTO "Products" ("name", "description", "price", "barcode", "category_id", "created_at") VALUES ('%1', '%2', %3, '%4', 3, '%5'))";

    ASSERT_GE(log.count(), 4);
    EXPECT_EQ(log.at(1).toStdString(), saleQuery.arg(root.field("created_at").toString()).toStdString());
    EXPECT_EQ(log.at(2).toStdString(), productQuery.arg("Top Orange", "Orange savour - artificial thing").arg(19.90).arg("335453576345", models.at(0).field("created_at").toString()).toStdString());
    EXPECT_EQ(log.at(3).toStdString(), productQuery.arg("Top Grenadine", "Strange red savour - artificial thing").arg(39.90).arg("587463576345", models.at(1).field("created_at").toString()).toStdString());
    EXPECT_EQ(log.count(), 4);
}

TEST_F(HasManyRelationTest, SuccessfulRead)
{
    EXPECT_TRUE(root.load({ "products" }));

    const QJsonObject category = root.jsonObject();

    const QJsonArray products = category.value("products").toArray();
    ASSERT_GE(products.count(), 2);

    QJsonObject product;

    product = products.at(0).toObject();
    ASSERT_EQ(product.value("id").toInt(), 1);
    ASSERT_EQ(product.value("name").toString().toStdString(), "Apple");

    product = products.at(1).toObject();
    ASSERT_EQ(product.value("id").toInt(), 2);
    ASSERT_EQ(product.value("name").toString().toStdString(), "Banana");

    EXPECT_EQ(products.count(), 2);

    ASSERT_GE(log.count(), 2);

    QString saleQuery = R"(SELECT * FROM "Categories" WHERE "id" = 1 LIMIT 1)";
    ASSERT_EQ(log.at(0).toStdString(), saleQuery.toStdString());

    QString itemsQuery = R"(SELECT * FROM "Products" WHERE "category_id" = 1)";
    ASSERT_EQ(log.at(1).toStdString(), itemsQuery.toStdString());

    EXPECT_EQ(log.count(), 2);
}

TEST_F(HasManyRelationTest, SuccessfulUpdate)
{
    ASSERT_TRUE(root.load({ "products" }));

    QJsonObject category = root.jsonObject();

    QJsonArray productsArray = category.value("products").toArray();
    productsArray.removeLast();

    QJsonObject productObject;

    productObject = productsArray.at(0).toObject();
    productsArray.replace(0, productObject);

    productObject = QJsonObject();
    productObject.insert("id", 3);
    productsArray.append(productObject);

    category.insert("items", productsArray);

    root.fill(category);
    EXPECT_TRUE(root.update());

    QList<Model> products = root.relation("products").models();
    ASSERT_GE(products.count(), 2);

    ASSERT_GE(log.count(), 6);

    QString saleQuery = R"(UPDATE "Categories" SET "name" = 'Fruits', "description" = 'Natural meals', "created_at" = '%1', "updated_at" = '%2' WHERE "id" = 1)";
    EXPECT_EQ(log.at(2).toStdString(), saleQuery.arg(root.field("created_at").toString(), root.field("updated_at").toString()).toStdString());

    {
        int id;
        QString name;
        QString description;
        double price;
        QString barcode;
        QString createdAt;
        QString updatedAt;

        auto switchToProduct = [&](int index, int outputIndex = -1) {
            const QJsonObject product = productsArray.at(index).toObject();
            id = product.value("id").toInt();
            name = product.value("name").toString();
            description = product.value("description").toString();
            price = product.value("price").toDouble();
            barcode = product.value("barcode").toString();

            if (outputIndex == -1) {
                createdAt = product.value("created_at").toString();
                updatedAt = product.value("updated_at").toString();
            } else {
                const Model product = products.at(outputIndex);
                createdAt = product.field("created_at").toString();
                updatedAt = product.field("updated_at").toString();
            }
        };

        QString itemQuery = R"(UPDATE "Products" SET "name" = '%2', "description" = '%3', "price" = %4, "barcode" = '%5', "category_id" = 1, "created_at" = '%6', "updated_at" = '%7' WHERE "id" = %1)";

        switchToProduct(0, 0);
        EXPECT_EQ(log.at(3).toStdString(), itemQuery.arg(id).arg(name, description).arg(price).arg(barcode, createdAt, updatedAt).toStdString());

        switchToProduct(1, 1);
        id = 2;
        name = "Banana";
        description = "Organic banana";
        price = 0.3;
        barcode = "2234567890123";
        EXPECT_EQ(log.at(4).toStdString(), itemQuery.arg(id).arg(name, description).arg(price).arg(barcode, createdAt, updatedAt).toStdString());
    }

    QString deleteQuery = R"(DELETE FROM "Products" WHERE "id" NOT IN (%1))";
    EXPECT_EQ(log.at(5).toStdString(), deleteQuery.arg("1, 2").toStdString());

    EXPECT_EQ(log.count(), 6);
}

TEST_F(HasManyRelationTest, SuccessfulDelete)
{
    EXPECT_TRUE(root.load({ "products" }));
    EXPECT_TRUE(root.deleteData());

    ASSERT_GE(log.count(), 5);

    QString saleQuery = R"(DELETE FROM "Products" WHERE "id" = %1)";
    EXPECT_EQ(log.at(2).toStdString(), saleQuery.arg(1).toStdString());
    EXPECT_EQ(log.at(3).toStdString(), saleQuery.arg(2).toStdString());

    QString itemsQuery = R"(DELETE FROM "Categories" WHERE "id" = 1)";
    EXPECT_EQ(log.at(4).toStdString(), itemsQuery.toStdString());

    EXPECT_EQ(log.count(), 5);
}


