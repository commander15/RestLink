set(TMDB_BEARER_TOKEN "" CACHE STRING "TMDB Bearer token for TMDB examples")

set(CMAKE_AUTORCC ON)

if (EMSCRIPTEN)
    set(QT_FEATURE_webassembly_fetch 1)
endif()

link_libraries(RestLink)

add_compile_definitions(TMDB_BEARER_TOKEN="${TMDB_BEARER_TOKEN}")

message("QML: ${QT_QML_ROOT_PATH}")

add_subdirectory(tmdb-desktop)

if (RESTLINK_BUILD_SQL)
    add_subdirectory(custom)
endif()

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

    if (RESTLINK_SUPPORT_SQL)
        add_subdirectory(store)
    endif()
endif()

file(COPY ${PROJECT_SOURCE_DIR}/testdata/ DESTINATION ${PROJECT_BINARY_DIR}/testdata)
