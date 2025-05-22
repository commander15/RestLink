#ifndef MODELTEST_H
#define MODELTEST_H

#include "sqltest.h"

#include <data/model.h>

using namespace RestLink::Sql;

class ModelTest : public SqlTest
{
protected:
    Model model = Model("products", api);
};

#endif // MODELTEST_H
