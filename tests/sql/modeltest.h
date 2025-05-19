#ifndef MODELTEST_H
#define MODELTEST_H

#include "sqllog.h"

#include <gtest/gtest.h>

#include <api.h>
#include <data/model.h>

using namespace RestLink::Sql;

class ModelTest : public testing::Test
{
protected:
    Model model = Model("products", Api::api(QUrl(DB_URL)));
    SqlLog sqlLog;
};

#endif // MODELTEST_H
