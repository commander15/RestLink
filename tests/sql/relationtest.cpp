#include "relationtest.h"

RelationTest::RelationTest(const QString &resource)
    : api(Api::api(QUrl(DB_URL)))
    , root(resource, api)
{
    if (!root.isValid())
        assert("Invalid resouce !");
}

void RelationTest::initializeTest()
{
}

void RelationTest::SetUp()
{
    resetDatabase();
    if (root.get(1))
        ready = true;
    else
        ready = false;
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
