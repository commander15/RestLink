cm_add_executable(client main.cpp qml.qrc)

set_target_properties(client PROPERTIES
    ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
    ANDROID_EXTRA_LIBS         "${SSL_LIBS}"
    ANDROID_VERSION_NAME       "${RestLink_VERSION}"
    ANDROID_VERSION_CODE       1
)

target_link_libraries(client PRIVATE Qt::Quick)

cm_install(TARGETS client
    RUNTIME DESTINATION bin
)
