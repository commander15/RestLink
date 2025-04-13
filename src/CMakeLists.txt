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

set(RESTLINK_MODULES Core)

add_subdirectory(lib)

if (RESTLINK_BUILD_QML)
    list(APPEND RESTLINK_MODULES Qml)
    add_subdirectory(import)
endif()

if (RESTLINK_SUPPORT_SQL)
    list(APPEND RESTLINK_MODULES Sql)
    add_subdirectory(sql)
endif()

if (RESTLINK_SUPPORT_EXCEL)
    list(APPEND RESTLINK_MODULES Excel)
    add_subdirectory(excel)
endif()

if (RESTLINK_BUILD_CLI)
    add_subdirectory(cli)
endif()

include(CMakePackageConfigHelpers)

set(ROOT .)

configure_package_config_file(RestLinkConfig.cmake.in RestLinkConfig.cmake
    PATH_VARS ROOT
    INSTALL_DESTINATION lib/cmake/RestLink
)

write_basic_package_version_file(RestLinkConfigVersion.cmake
    VERSION ${RestLink_VERSION}
    COMPATIBILITY AnyNewerVersion
    ARCH_INDEPENDENT
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/RestLinkConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/RestLinkConfigVersion.cmake
    DESTINATION lib/cmake/RestLink
)

install(EXPORT RestLinkTargets
    NAMESPACE RestLink::
    DESTINATION lib/cmake/RestLink
)
