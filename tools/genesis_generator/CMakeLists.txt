# Collect source files
file(GLOB GENESIS_SOURCES *.cpp)
file(GLOB GENESIS_HEADERS *.hpp)

# Create an executable named 'genesis_generator'
add_executable(genesis_generator ${GENESIS_SOURCES} ${GENESIS_HEADERS})

# Include directories
target_include_directories(genesis_generator PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link libraries
target_link_libraries(genesis_generator
    cryptography
    common
    time_chain
    value_chain
    OpenSSL::Crypto
)

# Ensure dependencies
add_dependencies(genesis_generator cryptography common time_chain value_chain)
