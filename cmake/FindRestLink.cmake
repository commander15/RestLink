option(RESTLINK_FORCE_FETCH OFF "Update RestLink by fetching the latest version during each build")
option(RESTLINK_USE_DEV     OFF "Use development version of RestLink")

# Default branch
if (NOT RESTLINK_USE_DEV)
    set(RESTLINK_BRANCH "main")
else()
    set(RESTLINK_BRANCH "dev")
endif()

if (RestLink_FIND_VERSION_MAJOR)
    set(RESTLINK_BRANCH "restlink-${RestLink_FIND_VERSION_MAJOR}")
endif()

# Check if RestLink is already found or installed
if (NOT RESTLINK_FORCE_FETCH)
    find_package(RestLink ${RestLink_VERSION} COMPONENTS "${RestLink_FIND_COMPONENTS}" PATHS ${CMAKE_BINARY_DIR})
else()
    set(RestLink_FOUND FALSE)
endif()

# If RestLink is found, check for ZLIB (optional dependency or requirement for the project)
if (NOT RestLink_FOUND OR RESTLINK_FORCE_FETCH)
    # If RestLink is not found, download and build it using ExternalProject
    set(RESTLINK_GIT "https://github.com/commander15/RestLink.git")

    set(ARGS)
    foreach (Component ${RestLink_FIND_COMPONENTS})
        string(TOUPPER ${Component} COMPONENT)
        list(APPEND ARGS
            -DRESTLINK_BUILD_${COMPONENT}=TRUE
            -DRESTLINK_SUPPORT_${COMPONENT}=TRUE
        )
    endforeach()

    include(ExternalProject)
    ExternalProject_Add(RestLink
        GIT_REPOSITORY ${RESTLINK_GIT}
        GIT_TAG        origin/${RESTLINK_BRANCH}
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

    ExternalProject_Add_Step(RestLink preparation
        COMMAND ${CMAKE_COMMAND} -E echo "🚨 Fetching and building RestLink from ${RESTLINK_BRANCH}..."
        COMMAND ${CMAKE_COMMAND} -E echo "${RESTLINK_GIT}"
        DEPENDERS mkdir
    )

    ExternalProject_Add_Step(RestLink finalization
        COMMAND ${CMAKE_COMMAND} -E echo "🚀 RestLink is good to go! May your builds be smooth and your bugs be few!"
        DEPENDEES install
    )
endif()
