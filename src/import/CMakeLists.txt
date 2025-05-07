set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_add_qml_module(RestLinkQml
    URI    RestLink
    VERSION ${RestLink_VERSION_MAJOR}.${RestLink_VERSION_MINOR}
    TYPEINFO RestLink.qmltypes
    PLUGIN_TARGET RestLinkQml
    QML_FILES ApiParameter.qml
    SOURCES request.h request.cpp
    SOURCES api.h api.cpp
    SOURCES requestparameter.h requestparameter.cpp
    SOURCES types.h types.cpp
    QML_FILES RequestParameter.qml
)

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
