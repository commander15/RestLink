qt_add_plugin(RestLinkSql
    CLASS_NAME RestLink::Sql::Plugin
    # sqlplugin.h sqlplugin.cpp
    # sqlserver.h sqlserver_p.h sqlserver.cpp
    sqlutils.h sqlutils.cpp
)

target_sources(RestLinkSql
  PRIVATE
  modelmanager.h modelmanager.cpp
  crudinterface.h crudinterface.cpp
  model.h model.cpp
  querybuilder.h querybuilder.cpp
  jsonutils.h jsonutils.cpp
  debug.h debug.cpp
  api.h api.cpp
  plugin.h plugin.cpp
  relation.h relation.cpp
  relation_base_impl.h relation_base_impl.cpp
  relation_api_impl.h relation_api_impl.cpp
  modelcontroller.h modelcontroller.cpp
    # sqldatabase.h sqldatabase.cpp
    # sqldatabasemanager.h sqldatabasemanager.cpp
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
