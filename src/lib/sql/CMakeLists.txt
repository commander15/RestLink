target_headers(RestLink
    PUBLIC
        sqlresponse.h
    PRIVATE
        sqlhandler.h sqlhandler_p.h
        sqlresponse_p.h
        sqlutils.h
)

target_sources(RestLink
    PRIVATE
        sqlhandler.cpp
        sqlresponse.cpp
        sqlutils.cpp
)

target_link_libraries(RestLink PUBLIC Qt::Sql)

target_compile_definitions(RestLink
    PUBLIC
        RESTLINK_SQL
)
