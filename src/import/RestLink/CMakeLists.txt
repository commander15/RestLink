qt_add_qml_module(RestLinkQml
    URI    RestLink
    VERSION ${RestLink_VERSION_MAJOR}.${RestLink_VERSION_MINOR}
    QML_FILES
        Api.qml
        ApiRequest.qml
        ApiRequestParameter.qml
    SOURCES
        restlinkplugin.cpp
        restlinkapi.cpp
        restlinkapirequest.cpp
    PLUGIN_TARGET RestLinkQml
    NO_GENERATE_PLUGIN_SOURCE
)

target_link_libraries(RestLinkQml PRIVATE Qt::Qml RestLink)

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
