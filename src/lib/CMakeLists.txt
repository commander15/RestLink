add_library(RestLink ${RestLink_LIBRARY_TYPE})

set_target_properties(RestLink PROPERTIES
    OUTPUT_NAME RestLink
    VERSION     ${RestLink_VERSION}
)

target_include_directories(RestLink PUBLIC $<INSTALL_INTERFACE:include>)

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

configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h @ONLY)

target_headers(RestLink
    PUBLIC
        restlink.h
        global.h config.h debug.h
        apibase.h api.h
        requestinterface.h
        parameter.h parameterlist.h pathparameter.h queryparameter.h
        request.h responsebase.h response.h
        header.h body.h
        compressionutils.h fileutils.h
        abstractrequestinterceptor.h
        abstractrequesthandler.h
    PRIVATE
        apibase_p.h api_p.h
        parameter_p.h pathparameter_p.h queryparameter_p.h header_p.h body_p.h
        request_p.h response_p.h
        abstractrequesthandler_p.h
)

target_sources(RestLink
    PRIVATE
        debug.cpp
        apibase.cpp api.cpp
        requestinterface.cpp
        parameter.cpp pathparameter.cpp queryparameter.cpp
        request.cpp responsebase.cpp response.cpp
        header.cpp body.cpp
        compressionutils.cpp fileutils.cpp
        abstractrequestinterceptor.cpp
        abstractrequesthandler.cpp
)

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
