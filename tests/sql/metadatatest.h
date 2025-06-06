#ifndef METADATATEST_H
#define METADATATEST_H

#include "common/sqltest.h"

#include <data/model.h>

class MetadataTest : public SqlTest
{
public:
    void SetUp() override
    { model = Model("products", api); }

    Model model;
};

#endif // METADATATEST_H
