add_library(cryptography
    cryptography.cpp
    schnorr_signature.cpp
)

# Include directories
target_include_directories(cryptography PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${SECP256K1_INCLUDE_DIR}
)

# Ensure that cryptography depends on the secp256k1 external project
add_dependencies(cryptography secp256k1)

# Link libraries
target_link_libraries(cryptography
    PUBLIC
    common
    ${SECP256K1_LIBRARY}
    OpenSSL::Crypto
)
