target_headers(RestLink
    PUBLIC
        server.h
        serverresponse.h
    PRIVATE
        server_p.h
        serverresponse_p.h
)

target_sources(RestLink
    PRIVATE
        server.cpp
        serverresponse.cpp
)
