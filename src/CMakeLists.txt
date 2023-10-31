cm_add_package(restpkg
    NAME    RestLink
    VERSION ${RestLink_VERSION}
)

#cm_install(PACKAGES restpkg DESTINATION lib/cmake)

add_subdirectory(core)

if (RESTLINK_BUILD_QML_PLUGIN)
    add_subdirectory(import)
endif()
