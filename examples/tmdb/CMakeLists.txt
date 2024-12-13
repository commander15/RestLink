qt_add_executable(tmdb main.cpp)

qt_add_qml_module(tmdb
    URI TmdbApp
    QML_FILES
        main.qml
        MediaDelegate.qml
        TmdbApi.qml
)

set_target_properties(tmdb PROPERTIES
    ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
)

target_link_libraries(tmdb PRIVATE Qt::Quick)

install(TARGETS tmdb RUNTIME DESTINATION bin)
