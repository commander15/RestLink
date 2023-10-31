cm_add_plugin(restlinkqml
    NAME    RestLink
    VERSION ${RestLink_VERSION_MAJOR}.${RestLink_VERSION_MINOR}
    CPP_SOURCES
        restlinkplugin.cpp
        restlinkapi.cpp
        restlinkapirequest.cpp
    QML_SOURCES
        qmldir
        Api.qml
        ApiRequest.qml ApiRequestParameter.qml
)

target_link_libraries(restlinkqml PRIVATE Qt::Qml restlink)


cm_install(TARGETS restlinkqml
    #PACKAGE        restpkg
    LIBRARY        DESTINATION qml/RestLink
    RUNTIME        DESTINATION qml/RestLink
)
