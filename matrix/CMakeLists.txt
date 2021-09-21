# --------------------------------------------------------------------

set(LIB_TARGET whirl-matrix)

get_filename_component(LIB_INCLUDE_PATH ".." ABSOLUTE)
get_filename_component(LIB_PATH "." ABSOLUTE)

file(GLOB_RECURSE LIB_CXX_SOURCES ${LIB_PATH}/*.cpp)
file(GLOB_RECURSE LIB_HEADERS ${LIB_PATH}/*.hpp)

add_library(${LIB_TARGET} STATIC ${LIB_CXX_SOURCES} ${LIB_HEADERS})
target_include_directories(${LIB_TARGET} PUBLIC ${LIB_INCLUDE_PATH})

# --------------------------------------------------------------------

# Dependencies

target_link_libraries(${LIB_TARGET} PUBLIC whirl-frontend)

# --------------------------------------------------------------------

# Workaround for std::filesystem

if(WHIRL_NO_FS)
    message(STATUS "No std::filesystem")
    target_compile_definitions(whirl PUBLIC WHIRL_NO_FS=1)
endif()

# --------------------------------------------------------------------
