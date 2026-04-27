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

# QML Plugin
if (RESTLINK_BUILD_QML)
    list(APPEND RESTLINK_MODULES Qml)
    add_subdirectory(import)
endif()

# CLI
if (RESTLINK_BUILD_CLI)
    add_subdirectory(cli)
endif()

# Plugins

include(FetchContent)
set(RESTLINK_PLUGINS)

macro(fetch_plugin name macro repo tag enabled_var)
    if (${enabled_var})
        FetchContent_Declare(
            ${name}
            GIT_REPOSITORY "https://github.com/${repo}.git"
            GIT_TAG "${tag}"
        )

        set(${macro}_BUILD_TESTS ON)

        list(APPEND RESTLINK_PLUGINS ${name})
    endif()
endmacro()

fetch_plugin(
    RestLinkSQL RESTLINK_SQL
    "commander15/RestLinkSQL"
    "1cce235c7d37875691983c9adae6c65247ecf4f2"
    RESTLINK_SUPPORT_SQL
)

fetch_plugin(
    RestLinkExcel RESTLINK_EXCEL
    "commander15/RestLinkExcel"
    "d4619af9537e10cc9601a8dfc39b483bddd0e147"
    RESTLINK_SUPPORT_EXCEL
)

FetchContent_MakeAvailable(${RESTLINK_PLUGINS})

# CMake Package

include(CMakePackageConfigHelpers)

set(ROOT .)

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
    DESTINATION lib/cmake/RestLink
)

install(
    EXPORT RestLinkTargets
    NAMESPACE RestLink::
    DESTINATION lib/cmake/RestLink
)
