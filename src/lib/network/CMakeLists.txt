target_headers(RestLink
    PUBLIC
        networkmanager.h cache.h cookiejar.h
        httputils.h
    PRIVATE
        cache_p.h
        httputils_p.h
        networkresponse_p.h
)

target_sources(RestLink
    PRIVATE
        networkmanager.cpp cache.cpp cookiejar.cpp
        networkresponse.cpp
        httputils.cpp
)
