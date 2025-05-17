enable_testing()

find_package(GTest REQUIRED)

add_library(RestLinkTest STATIC main.cpp)
target_compile_definitions(RestLinkTest INTERFACE DATA_DIR="${PROJECT_SOURCE_DIR}/testdata")
target_link_libraries(RestLinkTest PUBLIC GTest::gtest RestLink INTERFACE Qt::Test)
link_libraries(RestLinkTest)

# add_subdirectory(core)

if (RESTLINK_SUPPORT_SQL)
    add_subdirectory(sql)
endif()
