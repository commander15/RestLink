qt_add_executable(tmdb main.cpp)

qt_add_qml_module(tmdb
    URI TmdbApp
    RESOURCE_PREFIX /qml
    IMPORTS "RestLink/1.0"
    IMPORT_PATH "${RestLink_BINARY_DIR}/qml"
    QML_FILES
    Main.qml
        MediaDelegate.qml
        TmdbApi.qml
)

set_target_properties(tmdb PROPERTIES
    ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
)

target_link_libraries(tmdb PRIVATE Qt::Quick RestLink)

install(TARGETS tmdb RUNTIME DESTINATION bin)
