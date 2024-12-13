cmake_minimum_required(VERSION 3.20)

project(RestLink
    DESCRIPTION "Powerfull Rest Client for Qt"
    VERSION     1.1.0
    LANGUAGES   CXX C
)

option(RESTLINK_BUILD_QML_PLUGIN OFF "Build QML Plugin")
option(RESTLINK_BUILD_EXAMPLES   OFF "Build Examples")

set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml)

set(CMAKE_AUTOMOC ON)

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Qt6 REQUIRED COMPONENTS Network Quick)

qt_policy(SET QTP0001 NEW)

include(cmake/Utils.cmake)

add_subdirectory(src)

#add_subdirectory(test)

if (RESTLINK_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
