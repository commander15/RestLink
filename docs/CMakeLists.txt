# Finding doxygen
find_program(DOXYGEN_EXECUTABLE doxygen)

# Finding qhelpgenerator
file(REAL_PATH "${Qt6_DIR}/../../../" QtRootDir)
message(STATUS "Qt root directory: ${QtRootDir}")

configure_file(Doxyfile.in Doxyfile @ONLY)

find_program(QHG_EXECUTABLE qhelpgenerator PATHS ${QtRootDir}/bin ${QtRootDir}/libexec)

if (DOXYGEN_EXECUTABLE)
    add_custom_target(docs
        ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
        SOURCES Doxyfile.in
    )
endif()

if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/html)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION doc)

    if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/restlink.qch)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/restlink.qch DESTINATION doc)
    endif()
endif()

if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/latex)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/latex DESTINATION doc)
endif()
