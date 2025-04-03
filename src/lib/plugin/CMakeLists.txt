target_headers(RestLink
    PUBLIC
        pluginmanager.h
        plugin.h
    PRIVATE
        pluginmanager_p.h
)

target_sources(RestLink
    PRIVATE
        pluginmanager.cpp
        plugin.cpp
)
