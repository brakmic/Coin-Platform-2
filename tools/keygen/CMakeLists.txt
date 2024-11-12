# Create an executable named 'keygen' from 'keygen.cpp'
add_executable(keygen keygen.cpp)

# Include directories
target_include_directories(keygen PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(keygen
    cryptography
    common
    OpenSSL::Crypto
)

# Ensure 'keygen' depends on 'cryptography' and 'common' libraries
add_dependencies(keygen cryptography common)
