# Add library target for storage
add_library(storage
    storage_interface.hpp
    file_storage.hpp
    file_storage.cpp
    file_storage.tpp
)

target_include_directories(storage PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(storage
    common
)
