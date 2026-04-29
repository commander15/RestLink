if (BUILD_SHARED_LIBS)
    set(RestLink_LIBRARY_TYPE SHARED)
else()
    set(RestLink_LIBRARY_TYPE STATIC)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" AND FALSE)
    string(PREPEND CMAKE_STATIC_LIBRARY_SUFFIX "d")
    string(PREPEND CMAKE_SHARED_LIBRARY_SUFFIX "d")
    string(PREPEND CMAKE_SHARED_MODULE_SUFFIX  "d")
endif()

add_subdirectory(lib)

# QML Plugin
if (RESTLINK_BUILD_QML)
    add_subdirectory(import)
endif()

# CLI
if (RESTLINK_BUILD_CLI)
    add_subdirectory(cli)
endif()

# CMake Package

include(CMakePackageConfigHelpers)

set(ROOT .)

set(RESTLINK_MODULES)

if (RESTLINK_BUILD_TESTS)
    list(APPEND RESTLINK_MODULES Test)
endif()

configure_package_config_file(
    RestLinkConfig.cmake.in RestLinkConfig.cmake
    PATH_VARS ROOT
    INSTALL_DESTINATION lib/cmake/RestLink
)

write_basic_package_version_file(
    RestLinkConfigVersion.cmake
    VERSION ${RestLink_VERSION}
    COMPATIBILITY AnyNewerVersion
    ARCH_INDEPENDENT
)

install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/RestLinkConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/RestLinkConfigVersion.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/RestLinkMacros.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/RestLinkGTest.cmake
    DESTINATION lib/cmake/RestLink
)

install(
    EXPORT RestLinkTargets
    NAMESPACE RestLink::
    DESTINATION lib/cmake/RestLink
)
