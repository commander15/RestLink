if (RESTLINK_BUILD_QML_PLUGIN)
    set(QML_IMPORT_PATH
        ${CMAKE_BINARY_DIR}/qml
        CACHE STRING "" FORCE
    )

    add_subdirectory(tmdb)
    add_subdirectory(blagues)
endif()
