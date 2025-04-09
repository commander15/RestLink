qt_add_plugin(RestLinkSql
    CLASS_NAME RestLink::SqlPlugin
    sqlplugin.h sqlplugin.cpp
    sqlserver.h sqlserver_p.h sqlserver.cpp
    sqlutils.h sqlutils.cpp
)

target_sources(RestLinkSql
  PRIVATE
    sqldatabase.h sqldatabase.cpp
    sqldatabasemanager.h sqldatabasemanager.cpp
)

set(OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/restlink)
set_target_properties(RestLinkSql PROPERTIES
    OUTPUT_NAME restlinksql
    PREFIX      ""
    ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}
    LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}
)

target_link_libraries(RestLinkSql PUBLIC Qt::Sql RestLink)

set(INSTALL_DIR plugins/restlink)
install(TARGETS RestLinkSql
    EXPORT RestLinkTargets
    ARCHIVE DESTINATION ${INSTALL_DIR}
    LIBRARY DESTINATION ${INSTALL_DIR}
    RUNTIME DESTINATION ${INSTALL_DIR}
)
