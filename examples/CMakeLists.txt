if (RESTLINK_BUILD_QML_PLUGIN)
    set(QML_IMPORT_PATH
        ${CMAKE_BINARY_DIR}/qml
        CACHE STRING "" FORCE
    )

    set(SSL_LIBS libcrypto_1_1.so libssl_1_1.so)
    list(TRANSFORM SSL_LIBS PREPEND ${RestLink_SOURCE_DIR}/libs/${ANDROID_ABI}/)

    add_subdirectory(client)
    add_subdirectory(tmdb)
    add_subdirectory(blagues)
endif()
