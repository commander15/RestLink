#ifndef RELATIONTEST_H
#define RELATIONTEST_H

#include "sqltest.h"

#include <gtest/gtest.h>

#include <api.h>
#include <data/model.h>
#include <data/relation.h>

using namespace RestLink::Sql;

void resetDatabase();

class RelationTest : public SqlTest
{
protected:
    RelationTest(const QString &resource, int configIndex);

    void SetUp() override;

    QString creationTimestamp() const;
    QString creationTimestamp(const QString &relation) const;

    QString updateTimestamp() const;
    QString updateTimestamp(const QString &relation) const;

    Model root;
};

#endif // RELATIONTEST_H
