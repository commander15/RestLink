cm_add_executable(blagues main.cpp qml.qrc)

set_target_properties(blagues PROPERTIES
    ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
    ANDROID_EXTRA_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/libs/libcrypto_1_1.so;${CMAKE_CURRENT_SOURCE_DIR}/libs/libssl_1_1.so"
    ANDROID_VERSION_NAME "2.0"
    ANDROID_VERSION_CODE 7
)

target_link_libraries(blagues PRIVATE Qt::Quick)

cm_install(TARGETS blagues
    RUNTIME DESTINATION bin
)
