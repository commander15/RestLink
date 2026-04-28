qt_add_library(RestLink ${RestLink_LIBRARY_TYPE})

set_target_properties(RestLink PROPERTIES
    OUTPUT_NAME RestLink
    VERSION     ${RestLink_VERSION}
)

target_include_directories(RestLink
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<INSTALL_INTERFACE:include/RestLink>
)

target_compile_definitions(RestLink
    PUBLIC
        RESTLINK_LIB RESTLINK_${RestLink_LIBRARY_TYPE}
    PRIVATE
        RESTLINK_BUILD
)

target_link_libraries(RestLink PUBLIC Qt::Core Qt::Network)

if (ZLIB_FOUND)
    target_compile_definitions(RestLink PUBLIC ZLIB_LIB)
    target_link_libraries(RestLink PUBLIC ZLIB::ZLIB)
endif()

configure_file(core/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/core/config.h @ONLY)

target_headers(RestLink
    PUBLIC
        restlink.h
)

add_subdirectory(core)
add_subdirectory(utils)
add_subdirectory(network)
add_subdirectory(plugin)
add_subdirectory(server)

install(TARGETS RestLink
    EXPORT RestLinkTargets
    ARCHIVE        DESTINATION lib
    LIBRARY        DESTINATION lib
    RUNTIME        DESTINATION bin
    PUBLIC_HEADER  DESTINATION include/RestLink
    #PRIVATE_HEADER DESTINATION include/RestLink/private
)

generate_target_headers(RestLink FOLDER RestLink)
