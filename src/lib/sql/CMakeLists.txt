target_headers(RestLink
    PUBLIC
        sqlserver.h
        sqlutils.h
    PRIVATE
        sqlserver_p.h
)

target_sources(RestLink
    PRIVATE
        sqlserver.cpp
        sqlutils.cpp
)

target_link_libraries(RestLink PUBLIC Qt::Sql)

target_compile_definitions(RestLink
    PUBLIC
        RESTLINK_SQL
)
