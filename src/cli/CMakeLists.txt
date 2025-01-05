add_executable(RestCli main.cpp
    app.h app.cpp
)

target_link_libraries(RestCli PRIVATE Qt::Core RestLink)

set_target_properties(RestCli PROPERTIES
    OUTPUT_NAME restcli
)

install(TARGETS RestCli
    EXPORT RestLinkTargets
    RUNTIME DESTINATION bin
)
