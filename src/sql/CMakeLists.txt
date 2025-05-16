set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_add_plugin(RestLinkSql
    CLASS_NAME RestLink::Sql::Plugin
    # sqlplugin.h sqlplugin.cpp
    # sqlserver.h sqlserver_p.h sqlserver.cpp
)

set(UTILS_SOURCES
  utils/jsonutils.h utils/jsonutils.cpp
  utils/sqlutils.h utils/sqlutils.cpp
  utils/querybuilder.h utils/querybuilder.cpp
)

set(META_SOURCES
  meta/parseddata.h meta/parseddata.cpp
  meta/resourceinfo.h meta/resourceinfo.cpp
  meta/relationinfo.h meta/relationinfo.cpp
)

set(ROUTING_SOURCES
  routing/router.h routing/router.cpp
  routing/modelcontroller.h routing/modelcontroller.cpp
)

set(DATA_SOURCES
  data/crudinterface.h data/crudinterface.cpp
  data/model.h data/model.cpp
  data/relation.h data/relation.cpp
)

set(RELATIONS_SOURCES
  relations/relation_api_impl.h relations/relation_api_impl.cpp
  relations/relation_base_impl.h relations/relation_base_impl.cpp
)

target_sources(RestLinkSql
  PRIVATE
    debug.h debug.cpp
    plugin.cpp
    api.h api.cpp
    ${DATA_SOURCES}
    ${RELATIONS_SOURCES}
    ${ROUTING_SOURCES}
    ${META_SOURCES}
    ${UTILS_SOURCES}
    meta/endpointinfo.h meta/endpointinfo.cpp
    meta/sqlqueryinfo.h meta/sqlqueryinfo.cpp
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

target_include_directories(RestLinkSql
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

set(INSTALL_DIR plugins/restlink)
install(TARGETS RestLinkSql
    EXPORT RestLinkTargets
    ARCHIVE DESTINATION ${INSTALL_DIR}
    LIBRARY DESTINATION ${INSTALL_DIR}
    RUNTIME DESTINATION ${INSTALL_DIR}
)
