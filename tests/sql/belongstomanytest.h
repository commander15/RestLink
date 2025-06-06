#ifndef BELONGSTOMANYTEST_H
#define BELONGSTOMANYTEST_H

#include <common/relationtest.h>

class BelongsToManyTest : public RelationTest
{
protected:
    BelongsToManyTest() : RelationTest("products", 3) {}
};

#endif // BELONGSTOMANYTEST_H
