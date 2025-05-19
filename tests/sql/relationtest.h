#ifndef RELATIONTEST_H
#define RELATIONTEST_H

#include "sqllog.h"

#include <gtest/gtest.h>

#include <api.h>
#include <data/model.h>
#include <data/relation.h>

using namespace RestLink::Sql;

void resetDatabase();

class RelationTest : public testing::Test
{
protected:
    RelationTest(const QString &resource);

    void initializeTest();

    void SetUp() override;

    QString creationTimestamp() const;
    QString creationTimestamp(const QString &relation) const;

    QString updateTimestamp() const;
    QString updateTimestamp(const QString &relation) const;

    bool ready = false;
    Api *api;
    Model root;
    SqlLog log;
};

#endif // RELATIONTEST_H
