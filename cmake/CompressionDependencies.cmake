# ZLIB
set(BUILD_SHARED_LIBS   OFF)
set(ZLIB_BUILD_EXAMPLES OFF)
set(ZLIB_BUILD_TESTS    OFF)
set(ZLIB_INSTALL        OFF)

FetchContent_Declare(
    ZLIB
    URL https://zlib.net/fossils/zlib-1.1.3.tar.gz
    EXCLUDE_FROM_ALL
    FIND_PACKAGE_ARGS 1.1.3
)

# ZSTD
set(ZSTD_BUILD_COMPRESSION ON)
set(ZSTD_BUILD_DECOMPRESSION ON)
set(ZSTD_BUILD_CONTRIB OFF)
set(ZSTD_BUILD_DICTBUILDER ON)
set(ZSTD_MULTITHREAD_SUPPORT ON)
set(ZSTD_BUILD_STATIC ON)
set(ZSTD_BUILD_SHARED OFF)
set(ZSTD_BUILD_TESTS OFF)
set(ZSTD_LEGACY_SUPPORT OFF)
set(ZSTD_LEGACY_LEVEL 0)
set(ZSTD_BUILD_DEPRECATED OFF)
set(ZSTD_BUILD_PROGRAMS OFF)
set(ZSTD_PROGRAMS_LINK_SHARED OFF)
set(ZSTD_ZLIB_SUPPORT OFF)
set(ZSTD_LZMA_SUPPORT OFF)
set(ZSTD_LZ4_SUPPORT OFF)

FetchContent_Declare(
    ZSTD
    GIT_REPOSITORY https://github.com/facebook/zstd.git
    GIT_TAG v1.5.6
    SOURCE_SUBDIR build/cmake
    EXCLUDE_FROM_ALL
    FIND_PACKAGE_ARGS 1.5.6 NAMES zstd
)

# We find/download them
FetchContent_MakeAvailable(ZLIB ZSTD)

if (NOT TARGET ZLIB::ZLIB)
    if (TARGET zlib)
        add_library(ZLIB::ZLIB ALIAS zlib)
    else()
        message(FATAL_ERROR "Something went wrong, we can't find the zlib target.")
    endif()
endif()

if (NOT TARGET zstd::libzstd_static)
    if (TARGET libzstd_static)
        add_library(zstd::libzstd_static ALIAS libzstd_static)
    else()
        message(FATAL_ERROR "Something went wrong, we can't find the libzstd_static target.")
    endif()
endif()
