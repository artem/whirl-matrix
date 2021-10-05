message(STATUS "Database example")

add_executable(whirl_example_db main.cpp)
target_link_libraries(whirl_example_db whirl-matrix)
