qt_add_library(RestLink ${RestLink_LIBRARY_TYPE})

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
        global.h config.h debug.h
        jsonutils.h compressionutils.h
        apibase.h api.h apicache.h
        apirequest.h apireply.h
        apirequestinterceptor.h
    PRIVATE
        apibase_p.h api_p.h
        apirequest_p.h apireply_p.h
)

target_sources(RestLink
    PRIVATE
        debug.cpp
        jsonutils.cpp compressionutils.cpp
        apibase.cpp api.cpp apicache.cpp
        apirequest.cpp apireply.cpp
        apirequestinterceptor.cpp
)

qt_extract_metatypes(RestLink
    OUTPUT_FILES RestLink_METATYPES
)

install(TARGETS RestLink
    EXPORT RestLinkTargets
    ARCHIVE        DESTINATION lib
    LIBRARY        DESTINATION lib
    RUNTIME        DESTINATION bin
    PUBLIC_HEADER  DESTINATION include/RestLink
    #PRIVATE_HEADER DESTINATION include/RestLink/private
)

generate_target_headers(RestLink FOLDER RestLink)
