# Enable testing
enable_testing()

# Add the test executable
add_executable(test_suite test_main.cpp)

# Include directories
target_include_directories(test_suite PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party/googletest/googletest/include
    ${SECP256K1_INCLUDE_DIR}
)

# Ensure that test_suite depends on secp256k1
add_dependencies(test_suite secp256k1)

# Link libraries
target_link_libraries(test_suite
    PRIVATE
    gtest
    gtest_main
    config
    common
    storage
    networking
    cryptography
    time_chain
    value_chain
    consensus
    OpenSSL::Crypto
    ${SECP256K1_LIBRARY}
)

# Set C++ standard
set_property(TARGET test_suite PROPERTY CXX_STANDARD 23)
set_property(TARGET test_suite PROPERTY CXX_STANDARD_REQUIRED ON)

# Add the tests to CTest
add_test(NAME coin_platform_tests COMMAND test_suite)
