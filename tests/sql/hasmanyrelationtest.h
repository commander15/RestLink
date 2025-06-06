#ifndef HASMANYRELATIONTEST_H
#define HASMANYRELATIONTEST_H

#include "common/relationtest.h"

class HasManyRelationTest : public RelationTest
{
protected:
    HasManyRelationTest() : RelationTest("categories", 3) {}
};

#endif // HASMANYRELATIONTEST_H
