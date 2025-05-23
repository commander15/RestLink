#ifndef MODELTEST_H
#define MODELTEST_H

#include "common/sqltest.h"

#include <data/model.h>

using namespace RestLink::Sql;

class ModelTest : public SqlTest
{
protected:
    ModelTest() : SqlTest(0) {}

    Model model = Model("products", api);
};

#endif // MODELTEST_H
