include(../src/RestLinkGTest.cmake)

add_library(RestLinkTest STATIC main.cpp main.h)
target_headers(RestLinkTest PUBLIC main.h)
target_link_libraries(RestLinkTest PUBLIC GTest::gtest Qt::Core INTERFACE Qt::Test RestLink)

install(
    TARGETS RestLinkTest
    EXPORT RestLinkTestTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/RestLinkTest
)

install(
    EXPORT RestLinkTestTargets
    NAMESPACE RestLink::
    DESTINATION lib/cmake/RestLink
)

add_library(RestLink::RestLinkTest ALIAS RestLinkTest)
