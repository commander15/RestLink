add_executable(RestCli main.cpp
    app.h app.cpp
)

set_target_properties(RestCli PROPERTIES
    OUTPUT_NAME restcli
)

target_link_libraries(RestCli PRIVATE Qt::Core RestLink)

target_compile_definitions(RestCli PRIVATE RESTLINK_ROOT="${CMAKE_BINARY_DIR}")

install(
    TARGETS RestCli
    EXPORT RestLinkTargets
    RUNTIME DESTINATION bin
)
