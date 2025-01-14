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

target_link_libraries(RestLink PUBLIC Qt::Network)

if (ZLIB_FOUND)
    target_compile_definitions(RestLink PRIVATE ZLIB_LIB)
    target_link_libraries(RestLink PRIVATE ZLIB::ZLIB)
endif()

configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h @ONLY)

target_headers(RestLink
    PUBLIC
        restlink.h
        global.h config.h debug.h
        apibase.h api.h
        requestinterface.h pathparameter.h queryparameter.h
        request.h response.h
        header.h body.h
        compressionutils.h
        requestinterceptor.h
        requesthandler.h
        server.h serverresponse.h
    PRIVATE
        apibase_p.h api_p.h
        queryparameter_p.h
        request_p.h response_p.h
        server_p.h serverresponse_p.h
)

target_sources(RestLink
    PRIVATE
        debug.cpp
        apibase.cpp api.cpp
        requestinterface.cpp pathparameter.cpp queryparameter.cpp
        request.cpp response.cpp
        header.cpp body.cpp
        compressionutils.cpp
        requestinterceptor.cpp
        requesthandler.cpp
        server.cpp serverresponse.cpp
)

add_subdirectory(network)
add_subdirectory(plugin)

install(TARGETS RestLink
    EXPORT RestLinkTargets
    ARCHIVE        DESTINATION lib
    LIBRARY        DESTINATION lib
    RUNTIME        DESTINATION bin
    PUBLIC_HEADER  DESTINATION include/RestLink
    #PRIVATE_HEADER DESTINATION include/RestLink/private
)

generate_target_headers(RestLink FOLDER RestLink)
