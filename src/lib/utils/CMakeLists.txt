target_headers(RestLink
    PUBLIC
        fileutils.h
        compressionutils.h
)

target_sources(RestLink
    PRIVATE
        fileutils.cpp
        compressionutils.cpp
)

include(FetchContent)

# ZLIB

set(ZLIB_REQUIRED_VERSION 1.3)
set(SKIP_INSTALL_ALL ON)

FetchContent_Declare(
    ZLIB
    GIT_REPOSITORY "https://github.com/madler/zlib.git"
    GIT_TAG "v${ZLIB_REQUIRED_VERSION}"
)

FetchContent_MakeAvailable(ZLIB)

set_target_properties(zlibstatic PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(RestLink PRIVATE zlibstatic)
target_compile_definitions(RestLink PUBLIC RESTLINK_SUPPORT_ZLIB)
