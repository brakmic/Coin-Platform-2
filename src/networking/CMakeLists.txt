add_library(networking
    network_manager.cpp
)

target_include_directories(networking PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party/asio/asio/include
)

target_link_libraries(networking
    PRIVATE
    common
    OpenSSL::Crypto
)

# Link Threads if necessary
if (UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(networking PRIVATE Threads::Threads)
endif()
