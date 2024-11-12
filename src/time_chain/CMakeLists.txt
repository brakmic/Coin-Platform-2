# Add library target for time_chain
add_library(time_chain
    time_block.hpp
    time_block.cpp
    time_chain.hpp
    time_chain.cpp
)

target_include_directories(time_chain PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(time_chain
    storage
    cryptography
    common
)
