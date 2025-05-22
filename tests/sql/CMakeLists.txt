add_executable(RestLinkSqlTest
    main.cpp sqllog.h sqllog.cpp
    querytest.cpp
    modeltest.h modeltest.cpp
    relationtest.h relationtest.cpp
    hasonerelationtest.h hasonerelationtest.cpp
    belongstoonerelationtest.h belongstoonerelationtest.cpp
    hasmanyrelationtest.h hasmanyrelationtest.cpp
    sqltest.h sqltest.cpp
    querytest.h
)

set(DATABASE_DIR  "${PROJECT_BINARY_DIR}/testdata/store")
set(DATABASE_FILE "database_%1.sqlite")

 set(DATABASE_URL "sqlite:memory/%1")
# set(DATABASE_URL "sqlite://${DATABASE_DIR}/${DATABASE_FILE}")

target_compile_definitions(RestLinkSqlTest PRIVATE
    DB_URL="${DATABASE_URL}"
    DB_DIR="${DATABASE_DIR}"
)

target_link_libraries(RestLinkSqlTest PRIVATE RestLinkSqlLib)

add_test(NAME SqlTest COMMAND RestLinkSqlTest)
