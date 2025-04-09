set(CMAKE_AUTORCC ON)

link_libraries(RestLink)

message("QML: ${QT_QML_ROOT_PATH}")

add_subdirectory(tmdb-desktop)
add_subdirectory(custom)

if (RESTLINK_BUILD_QML)
    link_libraries(RestLinkQml)

    set(QML_IMPORT_PATH
        ${CMAKE_BINARY_DIR}/qml
        CACHE STRING "" FORCE
    )

    set(SSL_LIBS libcrypto_1_1.so libssl_1_1.so)
    list(TRANSFORM SSL_LIBS PREPEND ${RestLink_SOURCE_DIR}/libs/${ANDROID_ABI}/)

    set(RestLink_LIBRARIES ${RestLink_BINARY_DIR}/lib/libRestLink.so)

    #add_subdirectory(client)
    add_subdirectory(tmdb)
    #add_subdirectory(blagues)
endif()
