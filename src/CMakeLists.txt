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

add_subdirectory(core)

if (RESTLINK_BUILD_QML_PLUGIN)
    add_subdirectory(import)
endif()

install(EXPORT RestLinkTargets
    NAMESPACE RestLink::
    DESTINATION lib/cmake/RestLink
)
