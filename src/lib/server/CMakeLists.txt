target_headers(RestLink
    PUBLIC
        server.h resourcecontroller.h
        serverrequest.h serverresponse.h
    PRIVATE
        server_p.h
        serverresponse_p.h
)

target_sources(RestLink
    PRIVATE
        server.cpp resourcecontroller.cpp
        serverrequest.cpp serverresponse.cpp
)
