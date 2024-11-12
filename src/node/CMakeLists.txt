# Add library target for node
add_library(node
    node.hpp
    node.cpp
)

target_include_directories(node PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(node
    consensus
    time_chain
    value_chain
    storage
    networking
    cryptography
    config
    common
)
