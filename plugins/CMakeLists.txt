include(FetchContent)
set(RESTLINK_PLUGINS)

function(fetch_plugin name)
    set(options)
    set(oneValueArgs MACRO REPO TAG BUILD_OPTION BUILD_DESCRIPTION)
    set(multiValueArgs)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGV})

    # No plugin support for WebAssembly
    if (WASM)
        return()
    endif()

    option(${ARG_BUILD_OPTION} "${ARG_BUILD_DESCRIPTION}" OFF)
    if (NOT ${ARG_BUILD_OPTION})
        return()
    endif()

    FetchContent_Declare(
        ${name}
        GIT_REPOSITORY "https://github.com/${ARG_REPO}.git"
        GIT_TAG "${ARG_TAG}"
    )

    set(${ARG_MACRO}_BUILD_TESTS ${RESTLINK_BUILD_TESTS} CACHE BOOL "" FORCE)

    set(RESTLINK_PLUGINS ${RESTLINK_PLUGINS};${name} PARENT_SCOPE)
endfunction()

fetch_plugin(
    RestLinkSQL
    MACRO RESTLINK_SQL
    REPO "commander15/RestLinkSQL"
    TAG "d1bf0f376be43ec6794bd238097ecfcfe714b17d"
    BUILD_OPTION RESTLINK_SUPPORT_SQL
    BUILD_DESCRIPTION "Enable SQL support"
)

fetch_plugin(
    RestLinkExcel
    MACRO RESTLINK_EXCEL
    REPO "commander15/RestLinkExcel"
    TAG "9d650fb0140153f870a0b95b5d1bcb636cb634fc"
    BUILD_OPTION RESTLINK_SUPPORT_EXCEL
    BUILD_DESCRIPTION "Enable Excel support"
)

include(../src/RestLinkMacros.cmake)

FetchContent_MakeAvailable(${RESTLINK_PLUGINS})
