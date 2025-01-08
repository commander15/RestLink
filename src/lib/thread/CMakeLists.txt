target_headers(RestLink
    PUBLIC
        threadednetworkhandler.h
    PRIVATE
        internalresponse.h internalresponse_p.h
        threadednetworkhandler_p.h
)

target_sources(RestLink
    PRIVATE
        internalresponse.cpp
        threadednetworkhandler.cpp
)
