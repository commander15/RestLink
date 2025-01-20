qt_add_qml_module(RestLinkQml
    URI    RestLink
    VERSION ${RestLink_VERSION_MAJOR}.${RestLink_VERSION_MINOR}
    TYPEINFO RestLink.qmltypes
    QML_FILES
        Api.qml
        Request.qml
        RequestParameter.qml
    SOURCES
        restlinkplugin.cpp
    PLUGIN_TARGET RestLinkQml
    NO_GENERATE_PLUGIN_SOURCE
    SOURCES request.h request.cpp
    SOURCES api.h api.cpp
)

target_link_libraries(RestLinkQml PUBLIC Qt::Qml RestLink)

qt_query_qml_module(RestLinkQml
    QMLDIR        DIR
    QML_FILES     FILES
    TYPEINFO      TYPES
    PLUGIN_TARGET TARGET
)

install(FILES ${DIR} ${FILES} ${TYPES} DESTINATION qml/RestLink)

install(TARGETS ${TARGET}
    EXPORT RestLinkTargets
    ARCHIVE DESTINATION qml/RestLink
    LIBRARY DESTINATION qml/RestLink
    RUNTIME DESTINATION qml/RestLink
)
