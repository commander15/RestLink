cmake_minimum_required(VERSION 3.20)

project(RestLink
    DESCRIPTION "Powerfull Rest Client for Qt"
    VERSION     1.0.0
    LANGUAGES   CXX C
)

option(RESTLINK_BUILD_QML_PLUGIN OFF "Build QML Plugin")
option(RESTLINK_BUILD_EXAMPLES   OFF "Build Examples")

find_package(Cm REQUIRED COMPONENTS QtCore QtGui)
find_package(Qt5 REQUIRED COMPONENTS Network ${CmQtModules})

add_subdirectory(src)

if (RESTLINK_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
