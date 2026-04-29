set(GTEST_VERSION 1.14.0)

include(FetchContent)

FetchContent_Declare(
    GTest
    GIT_REPOSITORY "https://github.com/google/googletest.git"
    GIT_TAG "v${GTEST_VERSION}"
    FIND_PACKAGE_ARGS NAMES GTest ${GTEST_VERSION}
)

FetchContent_MakeAvailable(GTest)

if (TARGET gtest AND NOT TARGET GTest::gtest)
    add_library(GTest::gtest ALIAS gtest)
endif()
