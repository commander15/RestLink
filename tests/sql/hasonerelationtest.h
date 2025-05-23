#ifndef HASONERELATIONTEST_H
#define HASONERELATIONTEST_H

#include "common/relationtest.h"

class HasOneRelationTest : public RelationTest
{
protected:
    HasOneRelationTest() : RelationTest("products", 1) {}
};

#endif // HASONERELATIONTEST_H
