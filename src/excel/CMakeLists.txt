qt_add_plugin(RestLinkExcel
    CLASS_NAME RestLink::Excel::Plugin
    plugin.cpp
)

target_sources(RestLinkExcel
  PRIVATE
    worker.h worker.cpp
    documentmanager.h documentmanager.cpp
    document.h document.cpp
)

set(OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/plugins/restlink)
set_target_properties(RestLinkExcel PROPERTIES
    OUTPUT_NAME restlinkexcel
    PREFIX      ""
    ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}
    LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}
)

include(FetchContent)

FetchContent_Declare(
    QXlsx
    GIT_REPOSITORY "https://github.com/QtExcel/QXlsx.git"
    GIT_TAG        "v1.5.0"
    SOURCE_SUBDIR QXlsx
)

FetchContent_MakeAvailable(QXlsx)

target_link_libraries(RestLinkExcel PUBLIC QXlsx RestLink)

set(INSTALL_DIR plugins/restlink)
install(TARGETS RestLinkExcel
    EXPORT RestLinkTargets
    ARCHIVE DESTINATION ${INSTALL_DIR}
    LIBRARY DESTINATION ${INSTALL_DIR}
    RUNTIME DESTINATION ${INSTALL_DIR}
)
