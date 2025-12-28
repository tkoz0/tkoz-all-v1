find_package(Doxygen)

if (DOXYGEN_FOUND)
    set(DOXYGEN_INPUT
        "${CMAKE_SOURCE_DIR}"
    )
    string(JOIN " " DOXYGEN_INPUT "${DOXYGEN_INPUT}")
    configure_file(
        "${CMAKE_SOURCE_DIR}/docs/Doxyfile.in"
        "${CMAKE_BINARY_DIR}/Doxyfile"
        @ONLY
    )
    add_custom_target(docs
        COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_BINARY_DIR}/Doxyfile"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Generating documentation with Doxygen"
    )
else()
    message(STATUS "Doxygen not found, not building docs")
endif()
