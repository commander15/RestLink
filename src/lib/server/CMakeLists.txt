target_headers(RestLink
    PUBLIC
        server.h serverrequest.h serverresponse.h
        abstractserverworker.h
        abstractcontroller.h resourcecontroller.h
    PRIVATE
        server_p.h serverresponse_p.h
        abstractserverworker_p.h
)

target_sources(RestLink
    PRIVATE
        server.cpp serverrequest.cpp serverresponse.cpp
        abstractserverworker.cpp
        abstractcontroller.cpp resourcecontroller.cpp
)
