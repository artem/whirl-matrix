message(STATUS "Echo example")

add_executable(whirl_example_echo main.cpp)
target_link_libraries(whirl_example_echo whirl-matrix)
