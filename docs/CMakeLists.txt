find_program(DOXYGEN_EXECUTABLE doxygen)

configure_file(Doxyfile.in Doxyfile @ONLY)

if (DOXYGEN_EXECUTABLE)
    add_custom_target(docs
        ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
        SOURCES Doxyfile.in
    )
endif()
