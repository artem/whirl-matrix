message(STATUS "KV example")

# --------------------------------------------------------------------

# Node

add_library(whirl_example_kv_node
        node/main.hpp
        node/main.cpp)

target_link_libraries(whirl_example_kv_node
        whirl-frontend)

# --------------------------------------------------------------------

# Test

add_executable(whirl_example_kv main.cpp)

target_link_libraries(whirl_example_kv
        whirl-matrix
        whirl_example_kv_node)
