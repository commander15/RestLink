cm_add_library(restlink)

set_target_properties(restlink PROPERTIES
    OUTPUT_NAME RestLink
    VERSION     ${RestLink_VERSION}
)

target_link_libraries(restlink PUBLIC Qt::Network)

configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h @ONLY)

target_headers(restlink
    PUBLIC
        global.h config.h debug.h
        api.h
        apiconfigurationdownload.h
        apirequest.h apireply.h
    PRIVATE
        api_p.h
        apiconfigurationdownload_p.h
        apirequest_p.h apireply_p.h
)

target_sources(restlink
    PRIVATE
        debug.cpp
        api.cpp
        apiconfigurationdownload.cpp
        apirequest.cpp apireply.cpp
)

cm_install(TARGETS restlink
    #PACKAGE        restpkg
    ARCHIVE        DESTINATION lib
    LIBRARY        DESTINATION lib
    RUNTIME        DESTINATION bin
    PUBLIC_HEADER  DESTINATION include/RestLink
    PRIVATE_HEADER DESTINATION include/RestLink/private
)
