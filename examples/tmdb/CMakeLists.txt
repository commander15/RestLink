qt_add_executable(tmdb main.cpp)

qt_add_qml_module(tmdb
    URI TmdbApp
    VERSION 1.0
    RESOURCE_PREFIX /qml
    IMPORTS "RestLink/2.0"
    IMPORT_PATH "${RestLink_BINARY_DIR}/qml"
    QML_FILES
        Main.qml
        MediaDelegate.qml
        Tmdb.qml
        QML_FILES TmdbSearchRequest.qml
)

set_target_properties(tmdb PROPERTIES
    ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
)

target_link_libraries(tmdb PRIVATE Qt::Qml Qt::Quick RestLink)

install(TARGETS tmdb RUNTIME DESTINATION bin)
