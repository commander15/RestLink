#include <gtest/gtest.h>

#include <QtCore/qcoreapplication.h>

void init(QCoreApplication &app);
void cleanup(QCoreApplication &app);

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    QCoreApplication app(argc, argv);

    init(app);
    int exitCode = RUN_ALL_TESTS();
    cleanup(app);

    return exitCode;
}
