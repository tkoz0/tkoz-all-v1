find_program(CLANG_DOC_EXECUTABLE clang-doc)

# note: header only libraries must be included in some translation unit
# in order for clang-doc to document them

if(CLANG_DOC_EXECUTABLE)
    # clang-doc needs files, does not accept directories
    file(GLOB_RECURSE CLANG_DOC_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/*.h"
        "${CMAKE_SOURCE_DIR}/*.hpp"
        "${CMAKE_SOURCE_DIR}/*.c"
        "${CMAKE_SOURCE_DIR}/*.cpp"
    )
    if(CLANG_DOC_SOURCES)
        add_custom_target(docs-clang-doc
            COMMAND "${CLANG_DOC_EXECUTABLE}"
                -p "${CMAKE_BINARY_DIR}"
                --format=html
                --output="${CMAKE_BINARY_DIR}/docs-clang-doc"
                ${CLANG_DOC_SOURCES}
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            COMMENT "Generating documentation with clang-doc"
        )
    else()
        message(STATUS "No source files found for clang-doc")
    endif()
else()
    message(STATUS "Could not found clang-doc, not building docs")
endif()
