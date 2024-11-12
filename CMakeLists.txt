cmake_minimum_required(VERSION 3.20)
project(coin_platform2 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Include directories
include_directories(
    ${OPENSSL_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/asio/asio/include
)

# Add third_party libraries first
add_subdirectory(third_party)

# Add subdirectories for your modules
add_subdirectory(src/config)
add_subdirectory(src/common)
add_subdirectory(src/storage)
add_subdirectory(src/networking)
add_subdirectory(src/cryptography)
add_subdirectory(src/time_chain)
add_subdirectory(src/value_chain)
add_subdirectory(src/consensus)
add_subdirectory(src/node)

# Tools
add_subdirectory(tools)

# Add the main executable
add_executable(coin_platform2 src/main.cpp)

# Link the necessary libraries to the main executable
target_link_libraries(coin_platform2
    PRIVATE
    config
    common
    storage
    networking
    cryptography
    time_chain
    value_chain
    consensus
    node
    OpenSSL::Crypto
)

# Add the tests directory
add_subdirectory(tests)
