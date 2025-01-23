qt_add_plugin(RestLinkExcel
    CLASS_NAME RestLink::ExcelPlugin
    excelplugin.h excelplugin.cpp
)

target_sources(RestLinkExcel
  PRIVATE
    excelserver.h excelserver.cpp
    excelserver_p.h
    exceldocument.h exceldocument.cpp
    excelsheet.h excelsheet.cpp
)

set(OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/restlink)
set_target_properties(RestLinkExcel PROPERTIES
    OUTPUT_NAME restlinkexcel
    PREFIX      ""
    ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}
    LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}
)

target_link_libraries(RestLinkExcel PUBLIC QXlsx::QXlsx RestLink)

set(INSTALL_DIR plugins/restlink)
install(TARGETS RestLinkExcel
    EXPORT RestLinkTargets
    ARCHIVE DESTINATION ${INSTALL_DIR}
    LIBRARY DESTINATION ${INSTALL_DIR}
    RUNTIME DESTINATION ${INSTALL_DIR}
)
