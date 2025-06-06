#ifndef SQLTEST_H
#define SQLTEST_H

#include "sqllog.h"

#include <gtest/gtest.h>

#include <api.h>

using namespace RestLink::Sql;

class SqlTest : public testing::Test
{
protected:
    SqlTest(int configIndex = -1);
    virtual ~SqlTest();

    void SetUp() override;
    void TearDown() override;

    Api *api;
    SqlLog log;

private:
    static Api *createApi(int configIndex);

    static QHash<int, QJsonObject> s_configurations;

    friend void generateConfigurationFile();
};

#endif // SQLTEST_H
