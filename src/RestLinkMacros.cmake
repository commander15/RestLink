set(INSTALL_DIR plugins/restlink)
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/${INSTALL_DIR})

function(restlink_add_plugin name)
    set(options)
    set(oneValueArgs CLASS_NAME OUTPUT_NAME)
    set(multiValueArgs SOURCES LINK_LIBRARIES)

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT ARG_CLASS_NAME)
        message(FATAL_ERROR "RestLink: can't register plugin ${name}, CLASS_NAME required.")
    endif()

    if (NOT ARG_OUTPUT_NAME)
        string(TOLOWER ${name} ARG_OUTPUT_NAME)
    endif()

    qt_add_plugin(
        ${name}
        CLASS_NAME ${ARG_CLASS_NAME}
        ${ARG_SOURCES}
        ${ARG_UNPARSED_ARGUMENTS}
    )

    set_target_properties(
        ${name}
        PROPERTIES
            PREFIX ""
            OUTPUT_NAME ${ARG_OUTPUT_NAME}
            ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}
            LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}
            RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}
    )

    target_link_libraries(${name} PRIVATE RestLink::RestLink ${ARG_LINK_LIBRARIES})
endfunction()

function(restlink_install_plugin target)
    install(
        TARGETS ${target}
        ARCHIVE DESTINATION ${INSTALL_DIR}
        LIBRARY DESTINATION ${INSTALL_DIR}
        RUNTIME DESTINATION ${INSTALL_DIR}
    )
endfunction()

function(restlink_add_test name)
    qt_add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE RestLink::RestLinkTest)
    add_test(NAME ${name} COMMAND ${name})
endfunction()
