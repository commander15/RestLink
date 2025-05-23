#include "metadatatest.h"

#include <meta/resourceinfo.h>

TEST_F(MetadataTest, resourceInfoAreValid)
{
    const ResourceInfo resource = model.resourceInfo();
    ASSERT_TRUE(resource.isValid());

    ASSERT_EQ(resource.name().toStdString(), "products");
    ASSERT_EQ(resource.table().toStdString(), "Products");
    ASSERT_EQ(resource.primaryKey().toStdString(), "id");
    ASSERT_EQ(resource.localKey().toStdString(), "product_id");
    ASSERT_EQ(resource.creationTimestampField().toStdString(), "created_at");
    ASSERT_EQ(resource.updateTimestampField().toStdString(), "updated_at");

    const QStringList fillableFieldNames = resource.fillableFields();
    ASSERT_EQ(fillableFieldNames.count(), 4);
    ASSERT_EQ(fillableFieldNames.at(0).toStdString(), "name");
    ASSERT_EQ(fillableFieldNames.at(1).toStdString(), "description");
    ASSERT_EQ(fillableFieldNames.at(2).toStdString(), "price");
    ASSERT_EQ(fillableFieldNames.at(3).toStdString(), "barcode");

    const QStringList hiddenFields = resource.hiddenFields();
    ASSERT_EQ(hiddenFields.count(), 1);
    ASSERT_EQ(hiddenFields.at(0).toStdString(), "category_id");

    const QStringList fieldNames = resource.fieldNames();
    ASSERT_EQ(fieldNames.count(), 8);
    ASSERT_EQ(fieldNames.at(0).toStdString(), "id");
    ASSERT_EQ(fieldNames.at(1).toStdString(), "name");
    ASSERT_EQ(fieldNames.at(2).toStdString(), "description");
    ASSERT_EQ(fieldNames.at(3).toStdString(), "price");
    ASSERT_EQ(fieldNames.at(4).toStdString(), "barcode");
    ASSERT_EQ(fieldNames.at(5).toStdString(), "category_id");
    ASSERT_EQ(fieldNames.at(6).toStdString(), "created_at");
    ASSERT_EQ(fieldNames.at(7).toStdString(), "updated_at");

    ASSERT_EQ(resource.fieldType("id").id(), QMetaType::fromType<int>().id());
    ASSERT_EQ(resource.fieldType("name").id(), QMetaType::fromType<QString>().id());
    ASSERT_EQ(resource.fieldType("description").id(), QMetaType::fromType<QString>().id());
    ASSERT_EQ(resource.fieldType("price").id(), QMetaType::fromType<double>().id());
    ASSERT_EQ(resource.fieldType("barcode").id(), QMetaType::fromType<QString>().id());
    ASSERT_EQ(resource.fieldType("category_id").id(), QMetaType::fromType<int>().id());
    ASSERT_EQ(resource.fieldType("created_at").id(), QMetaType::fromType<QString>().id()); // TIMESTAMP are strings in Qt SQLite
    ASSERT_EQ(resource.fieldType("updated_at").id(), QMetaType::fromType<QString>().id()); // TIMESTAMP are strings in Qt SQLite
}

TEST_F(MetadataTest, relationInfosAreValid)
{
    const ResourceInfo resource = model.resourceInfo();
    RelationInfo relation;

    const QStringList relationNames = resource.relationNames();
    ASSERT_EQ(relationNames.count(), 3);
    ASSERT_TRUE(relationNames.contains("category"));
    ASSERT_TRUE(relationNames.contains("stock"));
    ASSERT_TRUE(relationNames.contains("sales"));

    relation = resource.relation("category");
    ASSERT_TRUE(relation.isValid());
    ASSERT_EQ(relation.name().toStdString(), "category");
    ASSERT_EQ(relation.table().toStdString(), "Categories");
    ASSERT_EQ(relation.localKey().toStdString(), "category_id");
    ASSERT_EQ(relation.foreignKey().toStdString(), "id");
    ASSERT_EQ(relation.autoLoadable(), true);
    ASSERT_EQ(relation.nestLoadable(), false);
    ASSERT_EQ(relation.type(), Relation::BelongsToOne);

    relation = resource.relation("stock");
    ASSERT_TRUE(relation.isValid());
    ASSERT_EQ(relation.name().toStdString(), "stock");
    ASSERT_EQ(relation.table().toStdString(), "Stocks");
    ASSERT_EQ(relation.localKey().toStdString(), "id");
    ASSERT_EQ(relation.foreignKey().toStdString(), "product_id");
    ASSERT_EQ(relation.autoLoadable(), true);
    ASSERT_EQ(relation.nestLoadable(), false);
    ASSERT_EQ(relation.type(), Relation::HasOne);

    relation = resource.relation("sales");
    ASSERT_TRUE(relation.isValid());
    ASSERT_EQ(relation.name().toStdString(), "sales");
    ASSERT_EQ(relation.table().toStdString(), "Sales");
    ASSERT_EQ(relation.pivot().toStdString(), "SaleItems");
    ASSERT_EQ(relation.localKey().toStdString(), "product_id");
    ASSERT_EQ(relation.foreignKey().toStdString(), "sale_id");
    ASSERT_EQ(relation.autoLoadable(), false);
    ASSERT_EQ(relation.nestLoadable(), false);
    ASSERT_EQ(relation.type(), Relation::BelongsToMany);
}
