cmake_minimum_required(VERSION 3.20)

project(RestLink
    DESCRIPTION "Powerfull Rest Client for Qt"
    VERSION     2.0.0
    LANGUAGES   CXX C
)

option(RESTLINK_SUPPORT_SQL    OFF "Enable SQL support")
option(RESTLINK_BUILD_DOCS     OFF "Build documentation")
option(RESTLINK_BUILD_CLI      OFF "Build cli tool")
option(RESTLINK_BUILD_QML      OFF "Build QML Plugin")
option(RESTLINK_BUILD_EXAMPLES OFF "Build Examples")

set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml)

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Qt REQUIRED COMPONENTS Core Network)

include(cmake/Utils.cmake)

add_subdirectory(src)

if (RESTLINK_BUILD_DOCS)
    add_subdirectory(docs)
endif()

#add_subdirectory(tests)

if (RESTLINK_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
