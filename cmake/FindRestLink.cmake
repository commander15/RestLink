# Default to latest version if no version specified
if (NOT RestLink_FIND_VERSION_MAJOR)
    set(RestLink_FIND_VERSION_MAJOR 2)
endif()

# Force Fetch
option(RESTLINK_FORCE_FETCH OFF "Update RestLink by fetching the latest version")

# Check if RestLink is already found or installed
find_package(RestLink ${RestLink_VERSION_MAJOR} COMPONENTS "${RestLink_FIND_COMPONENTS}" PATHS ${CMAKE_BINARY_DIR})

# Search ZLIB
find_package(ZLIB)
if (ZLIB_FOUND)
    message("Found ZLIB at ${ZLIB_DIR}")
endif()

# If RestLink is found, check for ZLIB (optional dependency or requirement for the project)
if (NOT RestLink_FOUND OR RESTLINK_FORCE_FETCH)
    # If RestLink is not found, download and build it using ExternalProject
    message(STATUS "RestLink not found. Fetching and building it...")
    
    set(ARGS)
    foreach (Component ${RestLink_FIND_COMPONENTS})
        string(TOUPPER ${Component} COMPONENT)
        list(APPEND ARGS -DRESTLINK_BUILD_${COMPONENT}=TRUE)
    endforeach()

    include(ExternalProject)
    ExternalProject_Add(RestLink
        GIT_REPOSITORY https://github.com/commander15/RestLink.git
        GIT_TAG        origin/restlink-${RestLink_FIND_VERSION_MAJOR}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_CXX_FLAGS_INIT=${CMAKE_CXX_FLAGS_INIT}
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
            -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
            -DQT_QMAKE_EXECUTABLE=${QT_QMAKE_EXECUTABLE}
            ${ARGS}
    )

    ExternalProject_Add_Step(RestLink finalization
        COMMAND ${CMAKE_COMMAND} -E echo "🚀 RestLink is good to go! May your builds be smooth and your bugs be few!"
        DEPENDEES install
    )
endif()

