# Add library target for common utilities
add_library(common
    types.hpp
    utilities.hpp
    utilities.cpp
)

target_include_directories(common PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
