# Include subdirectories
add_subdirectory(config)
add_subdirectory(common)
add_subdirectory(networking)
add_subdirectory(cryptography)
add_subdirectory(storage)
add_subdirectory(consensus)
add_subdirectory(time_chain)
add_subdirectory(value_chain)
add_subdirectory(node)

# Collect all source files
file(GLOB SRC_FILES *.cpp)

# Create executable
add_executable(coin_platform2 ${SRC_FILES})

# Link libraries
target_link_libraries(coin_platform2
    node
    consensus
    time_chain
    value_chain
    storage
    networking
    cryptography
    config
    common
    OpenSSL::Crypto
    ${SECP256K1_LIBRARY}
)
