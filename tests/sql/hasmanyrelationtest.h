#ifndef HASMANYRELATIONTEST_H
#define HASMANYRELATIONTEST_H

#include "common/relationtest.h"

class HasManyRelationTest : public RelationTest
{
public:
    HasManyRelationTest() : RelationTest("categories", 2) {}
};

#endif // HASMANYRELATIONTEST_H
