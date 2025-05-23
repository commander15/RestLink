#ifndef BELONGSTOONERELATIONTEST_H
#define BELONGSTOONERELATIONTEST_H

#include "common/relationtest.h"

class BelongsToOneRelationTest : public RelationTest
{
protected:
    BelongsToOneRelationTest() : RelationTest("products", 1) {}
};

#endif // BELONGSTOONERELATIONTEST_H
