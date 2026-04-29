#include <gtest/gtest.h>

#include <QtCore/qcoreapplication.h>

#include "main.h"

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    QCoreApplication app(argc, argv);

    init(app);
    int exitCode = RUN_ALL_TESTS();
    cleanup(app);

    return exitCode;
}
