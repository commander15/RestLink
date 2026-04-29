cmake_minimum_required(VERSION 3.30)

project(RestLink
    DESCRIPTION  "Powerfull Rest Client for Qt"
    VERSION      2.3.1
    LANGUAGES    CXX C
    HOMEPAGE_URL "commander15.github.io/RestLink-Docs"
)

option(RESTLINK_BUILD_CLI          ON  "Build CLI tool")
option(RESTLINK_BUILD_QML          ON  "Build QML Plugin")
option(RESTLINK_BUILD_TESTS        OFF "Build RestLink tests")
option(RESTLINK_BUILD_DOC          OFF "Build RestLink documentation")
option(RESTLINK_BUILD_TRANSLATIONS OFF "Build RestLink translations")
option(RESTLINK_BUILD_EXAMPLES     OFF "Build RestLink Examples")

if (NOT WASM)
    option(RESTLINK_SUPPORT_SQL   OFF "Enable SQL support")
    option(RESTLINK_SUPPORT_EXCEL OFF "Enable EXCEL support")
endif()

set(RESTLINK_LOCALES en fr)

# Only define global build settings if top-level
if (PROJECT_IS_TOP_LEVEL)

    if (NOT EMSCRIPTEN)
        option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
    else()
        option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
    endif()

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_STANDARD_REQUIRED TRUE)

endif()

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(cmake/Utils.cmake)

# Qt setup
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (PROJECT_IS_TOP_LEVEL)
    set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml)
    set(QML_IMPORT_PATH ${QT_QML_OUTPUT_DIRECTORY} CACHE STRING "" FORCE)
endif()

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

if (PROJECT_IS_TOP_LEVEL)
    qt_standard_project_setup(
        REQUIRES 6.9.0
        I18N_SOURCE_LANGUAGE      en
        I18N_TRANSLATED_LANGUAGES ${RESTLINK_LOCALES}
    )
endif()

if (Qt6Qml_FOUND)
    qt_policy(SET QTP0001 NEW)
endif()
qt_policy(SET QTP0002 NEW)
qt_policy(SET QTP0003 NEW)

find_package(ZLIB)

add_subdirectory(src)

if (PROJECT_IS_TOP_LEVEL)

    if (RESTLINK_BUILD_DOC)
        add_subdirectory(doc)
    endif()

    if (RESTLINK_BUILD_TESTS)
        add_subdirectory(tests)
    endif()

    if (RESTLINK_BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()

    # Packaging
    set(CPACK_PACKAGE_VENDOR "Amadou Benjamain")

    set(CPACK_GENERATOR ZIP)
    if (LINUX)
        list(APPEND CPACK_GENERATOR TGZ)

        find_program(DPKG_DEB NAMES dpkg-deb)
        if (DPKG_DEB)
            set(CPACK_DEBIAN_PACKAGE_DEPENDS    "libssl3t64 (>= 3.0.13-0ubuntu3.6)")
            set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Amadou Benjamain")
            list(APPEND CPACK_GENERATOR DEB)
        endif()
    endif()

    include(CPack)

    # CI/CD
    configure_file(
        .github/workflows/ci-cd.yml.in
        ${CMAKE_CURRENT_SOURCE_DIR}/.github/workflows/ci-cd.yml
        @ONLY
    )

endif()
