cmake_minimum_required(VERSION 3.14)
project(whirl-matrix)

include(cmake/CheckCompiler.cmake)

option(WHIRL_MATRIX_DEVELOPER "Matrix development mode" OFF)
option(WHIRL_MATRIX_EXAMPLES "Enable Matrix examples" OFF)

include(cmake/CompileOptions.cmake)
include(cmake/Sanitize.cmake)

add_subdirectory(third_party)

add_subdirectory(matrix)

if(WHIRL_MATRIX_DEVELOPER OR WHIRL_MATRIX_EXAMPLES)
    add_subdirectory(examples)
endif()
