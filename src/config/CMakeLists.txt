# Add library target for config
add_library(config
    config.hpp
    config.cpp
)

target_include_directories(config PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
