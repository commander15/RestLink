cmake_minimum_required(VERSION 3.20)

project(RestLink
    DESCRIPTION "Powerfull Rest Client for Qt"
    VERSION     2.2.0
    LANGUAGES   CXX C
)

option(RESTLINK_BUILD_CLI          ON  "Build CLI tool")
option(RESTLINK_BUILD_QML          ON  "Build QML Plugin")
option(RESTLINK_BUILD_TESTS        OFF "Build RestLink tests")
option(RESTLINK_BUILD_DOCS         ON  "Build RestLink documentation")
option(RESTLINK_BUILD_TRANSLATIONS OFF "Build RestLink translations")
option(RESTLINK_BUILD_EXAMPLES     OFF "Build RestLink Examples")

if (NOT WASM)
    option(RESTLINK_SUPPORT_SQL        OFF "Enable SQL support")
    option(RESTLINK_SUPPORT_EXCEL      OFF "Enable EXCEL support")
endif()

set(RESTLINK_LOCALES en fr)

# CMake settings
if (NOT EMSCRIPTEN)
    option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
else()
    option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
endif()

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# CXX Standard
set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED TRUE)

# C Standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
# Registering project module path
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Including utility scripts
include(cmake/Utils.cmake)

# Qt setup

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml)
set(QML_IMPORT_PATH ${QT_QML_OUTPUT_DIRECTORY} CACHE STRING "" FORCE)

set(Qt_MODULES Core Network)

if (RESTLINK_BUILD_QML)
    list(APPEND Qt_MODULES Qml)

    if (RESTLINK_BUILD_EXAMPLES)
        list(APPEND Qt_MODULES Quick)
    endif()
endif()

if (RESTLINK_BUILD_TESTS)
    list(APPEND Qt_MODULES Test)
endif()

if (RESTLINK_BUILD_TRANSLATIONS)
    list(APPEND Qt_MODULES LinguistTools)
endif()

if (RESTLINK_SUPPORT_SQL)
    list(APPEND Qt_MODULES Sql)
endif()

find_package(Qt6 REQUIRED COMPONENTS ${Qt_MODULES})

qt_standard_project_setup(
    REQUIRES 6.8.0
    I18N_SOURCE_LANGUAGE      en
    I18N_TRANSLATED_LANGUAGES ${RESTLINK_LOCALES}
)

if (Qt6Qml_FOUND)
    qt_policy(SET QTP0001 NEW)
endif()
qt_policy(SET QTP0002 NEW)
qt_policy(SET QTP0003 NEW)

# Other dependencies
find_package(ZLIB)

if (RESTLINK_SUPPORT_EXCEL)
    find_package(QXlsxQt6 REQUIRED)
endif()

include(cmake/Utils.cmake)

add_subdirectory(src)

if (RESTLINK_BUILD_DOCS)
    add_subdirectory(docs)
endif()

if (RESTLINK_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if (RESTLINK_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
