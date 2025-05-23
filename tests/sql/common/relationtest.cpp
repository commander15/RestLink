#include "relationtest.h"

RelationTest::RelationTest(const QString &resource, int configIndex)
    :SqlTest(configIndex)
    , root(resource, api)
{
    if (!root.isValid())
        assert("Invalid resouce !");
}

void RelationTest::SetUp()
{
    SqlTest::SetUp();
    root.get(1);
}

QString RelationTest::creationTimestamp() const
{
    return root.field("created_at").toString();
}

QString RelationTest::creationTimestamp(const QString &relation) const
{
    return root.relation(relation).model().field("created_at").toString();
}

QString RelationTest::updateTimestamp() const
{
    return root.field("updated_at").toString();
}

QString RelationTest::updateTimestamp(const QString &relation) const
{
    return root.relation(relation).model().field("updated_at").toString();
}
