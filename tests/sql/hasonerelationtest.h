#ifndef HASONERELATIONTEST_H
#define HASONERELATIONTEST_H

#include "common/relationtest.h"

class HasOneRelationTest : public RelationTest
{
protected:
    HasOneRelationTest() : RelationTest("products", 2) {}
};

#endif // HASONERELATIONTEST_H
