qt_add_executable(blagues main.cpp)

qt_add_qml_module(blagues
    URI Blagues
    QML_FILES
        main.qml
        AppSettings.qml
        BasicJokeView.qml
        JokeView.qml
        Joke.qml
        JokePage.qml
        MyToolBar.qml
        ShareOption.qml
    RESOURCES
        icons/blagues.svg
        icons/logo.png
        icons/share.png
        icons/share_image.png
        icons/share_text.png
    RESOURCE_PREFIX /
)

qt_target_qml_sources(blagues
    RESOURCES
        qtquickcontrols2.conf
    PREFIX "/"
)

set_target_properties(blagues PROPERTIES
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
    QT_ANDROID_EXTRA_LIBS "${SSL_LIBS};${RestLink_LIBRARIES}"
    QT_ANDROID_VERSION_NAME "2.0"
    QT_ANDROID_VERSION_CODE 7
)

target_link_libraries(blagues PRIVATE Qt::Quick)

install(TARGETS blagues RUNTIME DESTINATION bin)
