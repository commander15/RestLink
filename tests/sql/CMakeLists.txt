set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(RestLinkSqlTest
    common/main.cpp
    common/sqllog.h common/sqllog.cpp
    common/sqltest.h common/sqltest.cpp
    common/relationtest.h common/relationtest.cpp
    queryrunnertest.h queryrunnertest.cpp
    metadatatest.h metadatatest.cpp
    modeltest.h modeltest.cpp
    hasonerelationtest.h hasonerelationtest.cpp
    belongstoonerelationtest.h belongstoonerelationtest.cpp
    hasmanyrelationtest.h hasmanyrelationtest.cpp
    belongstomanytest.h belongstomanytest.cpp
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
