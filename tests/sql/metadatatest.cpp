#include "metadatatest.h"

#include <meta/endpointinfo.h>
#include <meta/sqlqueryinfo.h>
#include <meta/resourceinfo.h>

#include <QtCore/qfile.h>

TEST_F(MetadataTest, SuccessfulFinalConfigurationSaving)
{
    const QJsonDocument config(api->configuration());

    QFile file(QStringLiteral(DATA_DIR) + "/store/configuration_x.json");
    ASSERT_TRUE(file.open(QIODevice::WriteOnly));
    ASSERT_GE(file.write(config.toJson(QJsonDocument::Indented)), 1);
    ASSERT_TRUE(file.flush());
    file.close();
}

TEST_F(MetadataTest, RetrievesValidQueryInfo)
{
    const EndpointInfo endpoint = api->endpointInfo("/products-categories");
    EXPECT_TRUE(endpoint.isValid());
    EXPECT_EQ(endpoint.name().toStdString(), "/products-categories");
    EXPECT_TRUE(endpoint.hasGetQuery());

    const SqlQueryInfo query = endpoint.getQuery();
    EXPECT_TRUE(query.isValid());
    EXPECT_TRUE(query.isArrayQuery());

    const QStringList statements = query.statements();
    ASSERT_EQ(statements.count(), 2);
    EXPECT_EQ(statements.at(0).toStdString(), R"(SELECT * FROM Products LIMIT 10 OFFSET ({{page}} - 1) * 10)");
    EXPECT_EQ(statements.at(1).toStdString(), R"(SELECT * FROM Categories LIMIT 10 OFFSET ({{page}} - 1) * 10)");

    const QStringList parameters = query.parameterNames();
    ASSERT_EQ(parameters.count(), 1);
    EXPECT_EQ(parameters.at(0).toStdString(), "page");

    EXPECT_EQ(query.parameterValue("page").toString().toInt(), 1);

    QVariantHash data;
    data.insert("page", 2);

    const QStringList formated = query.allFormated(data);
    ASSERT_EQ(formated.count(), 2);
    EXPECT_EQ(formated.at(0).toStdString(), R"(SELECT * FROM Products LIMIT 10 OFFSET (2 - 1) * 10)");
    EXPECT_EQ(formated.at(1).toStdString(), R"(SELECT * FROM Categories LIMIT 10 OFFSET (2 - 1) * 10)");
}

TEST_F(MetadataTest, RetrievesValidResourceInfo)
{
    const ResourceInfo resource = model.resourceInfo();
    EXPECT_TRUE(resource.isValid());

    EXPECT_EQ(resource.name().toStdString(), "products");
    EXPECT_EQ(resource.table().toStdString(), "Products");
    EXPECT_EQ(resource.primaryKey().toStdString(), "id");
    EXPECT_EQ(resource.localKey().toStdString(), "product_id");
    EXPECT_EQ(resource.creationTimestampField().toStdString(), "created_at");
    EXPECT_EQ(resource.updateTimestampField().toStdString(), "updated_at");

    const QStringList fillableFieldNames = resource.fillableFields();
    ASSERT_EQ(fillableFieldNames.count(), 4);
    EXPECT_EQ(fillableFieldNames.at(0).toStdString(), "name");
    EXPECT_EQ(fillableFieldNames.at(1).toStdString(), "description");
    EXPECT_EQ(fillableFieldNames.at(2).toStdString(), "price");
    EXPECT_EQ(fillableFieldNames.at(3).toStdString(), "barcode");

    const QStringList hiddenFields = resource.hiddenFields();
    ASSERT_EQ(hiddenFields.count(), 1);
    EXPECT_EQ(hiddenFields.at(0).toStdString(), "category_id");

    const QStringList fieldNames = resource.fieldNames();
    ASSERT_EQ(fieldNames.count(), 8);
    EXPECT_EQ(fieldNames.at(0).toStdString(), "id");
    EXPECT_EQ(fieldNames.at(1).toStdString(), "name");
    EXPECT_EQ(fieldNames.at(2).toStdString(), "description");
    EXPECT_EQ(fieldNames.at(3).toStdString(), "price");
    EXPECT_EQ(fieldNames.at(4).toStdString(), "barcode");
    EXPECT_EQ(fieldNames.at(5).toStdString(), "category_id");
    EXPECT_EQ(fieldNames.at(6).toStdString(), "created_at");
    EXPECT_EQ(fieldNames.at(7).toStdString(), "updated_at");

    EXPECT_EQ(resource.fieldType("id").id(), QMetaType::fromType<int>().id());
    EXPECT_EQ(resource.fieldType("name").id(), QMetaType::fromType<QString>().id());
    EXPECT_EQ(resource.fieldType("description").id(), QMetaType::fromType<QString>().id());
    EXPECT_EQ(resource.fieldType("price").id(), QMetaType::fromType<double>().id());
    EXPECT_EQ(resource.fieldType("barcode").id(), QMetaType::fromType<QString>().id());
    EXPECT_EQ(resource.fieldType("category_id").id(), QMetaType::fromType<int>().id());
    EXPECT_EQ(resource.fieldType("created_at").id(), QMetaType::fromType<QString>().id()); // TIMESTAMP are strings in Qt SQLite
    EXPECT_EQ(resource.fieldType("updated_at").id(), QMetaType::fromType<QString>().id()); // TIMESTAMP are strings in Qt SQLite
}

TEST_F(MetadataTest, RetrievesValidRelationInfo)
{
    const ResourceInfo resource = model.resourceInfo();
    RelationInfo relation;

    const QStringList relationNames = resource.relationNames();
    ASSERT_EQ(relationNames.count(), 3);
    EXPECT_TRUE(relationNames.contains("category"));
    EXPECT_TRUE(relationNames.contains("stock"));
    EXPECT_TRUE(relationNames.contains("sales"));

    relation = resource.relation("category");
    EXPECT_TRUE(relation.isValid());
    EXPECT_EQ(relation.name().toStdString(), "category");
    EXPECT_EQ(relation.table().toStdString(), "Categories");
    EXPECT_EQ(relation.localKey().toStdString(), "category_id");
    EXPECT_EQ(relation.foreignKey().toStdString(), "id");
    EXPECT_EQ(relation.autoLoadable(), true);
    EXPECT_EQ(relation.nestLoadable(), false);
    EXPECT_EQ(relation.type(), Relation::BelongsToOne);

    relation = resource.relation("stock");
    EXPECT_TRUE(relation.isValid());
    EXPECT_EQ(relation.name().toStdString(), "stock");
    EXPECT_EQ(relation.table().toStdString(), "Stocks");
    EXPECT_EQ(relation.localKey().toStdString(), "id");
    EXPECT_EQ(relation.foreignKey().toStdString(), "product_id");
    EXPECT_EQ(relation.autoLoadable(), true);
    EXPECT_EQ(relation.nestLoadable(), false);
    EXPECT_EQ(relation.type(), Relation::HasOne);

    relation = resource.relation("sales");
    EXPECT_TRUE(relation.isValid());
    EXPECT_EQ(relation.name().toStdString(), "sales");
    EXPECT_EQ(relation.table().toStdString(), "Sales");
    EXPECT_EQ(relation.pivot().toStdString(), "SaleItems");
    EXPECT_EQ(relation.localKey().toStdString(), "product_id");
    EXPECT_EQ(relation.foreignKey().toStdString(), "sale_id");
    EXPECT_EQ(relation.autoLoadable(), false);
    EXPECT_EQ(relation.nestLoadable(), false);
    EXPECT_EQ(relation.type(), Relation::BelongsToMany);
}
