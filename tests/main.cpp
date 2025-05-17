#include <gtest/gtest.h>

#include <QtCore/qcoreapplication.h>

void init(QCoreApplication &app);

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    QCoreApplication app(argc, argv);
    init(app);
    return RUN_ALL_TESTS();
}
