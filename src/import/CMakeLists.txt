qt_add_qml_module(RestLinkQml
    URI    RestLink
    VERSION ${RestLink_VERSION_MAJOR}.${RestLink_VERSION_MINOR}
    TYPEINFO RestLink.qmltypes
    PLUGIN_TARGET RestLinkQml
    QML_FILES
        ApiParameter.qml
        RequestParameter.qml
        SOURCES core/utils.h core/utils.cpp
)

target_include_directories(RestLinkQml PRIVATE core ..)

set(CORE_SOURCES
    core/api.h core/api.cpp
    core/request.h core/request.cpp
    core/requestparameter.h core/requestparameter.cpp
    core/types.h core/types.cpp
)

set(LIB_HEADERS
    ../lib/api.h ../lib/apibase.h
    ../lib/response.h ../lib/responsebase.h
    ../lib/network/networkmanager.h
    ../lib/network/cookiejar.h
    ../lib/network/cache.h
)

source_group(lib FILES ${LIB_HEADERS})

target_sources(RestLinkQml PRIVATE ${CORE_SOURCES} ${LIB_HEADERS})

target_link_libraries(RestLinkQml PUBLIC Qt::Qml RestLink)

if (EMSCRIPTEN)
    return()
endif()

qt_query_qml_module(RestLinkQml
    QMLDIR        DIR
    QML_FILES     FILES
    TYPEINFO      TYPES
    PLUGIN_TARGET TARGET
)

install(FILES ${DIR} ${FILES} ${TYPES} DESTINATION qml/RestLink)

install(TARGETS ${TARGET}
    ARCHIVE DESTINATION qml/RestLink
    LIBRARY DESTINATION qml/RestLink
    RUNTIME DESTINATION qml/RestLink
)

install(FILES RestLinkQml.cmake DESTINATION lib/cmake/RestLink)
