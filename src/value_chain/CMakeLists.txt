# Add library target for value_chain
add_library(value_chain
    transaction.hpp
    transaction.cpp
    value_block.hpp
    value_block.cpp
    value_chain.hpp
    value_chain.cpp
)

target_include_directories(value_chain PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(value_chain
    storage
    cryptography
    common
)
