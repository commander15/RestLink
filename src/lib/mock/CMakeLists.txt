target_headers(RestLink
    PUBLIC
        mockmanager.h
        mockreply.h
    PRIVATE
        mockreply_p.h
)

target_sources(RestLink
    PRIVATE
        mockmanager.cpp
        mockreply.cpp
)
