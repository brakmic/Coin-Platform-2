include(ExternalProject)

# Set the installation directory for secp256k1
set(SECP256K1_INSTALL_DIR ${CMAKE_BINARY_DIR}/secp256k1_install)

ExternalProject_Add(
    secp256k1
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/secp256k1
    CONFIGURE_COMMAND ./autogen.sh && ./configure --enable-module-schnorrsig --enable-experimental --enable-module-ecdh --prefix=${SECP256K1_INSTALL_DIR}
    BUILD_COMMAND $(MAKE)
    INSTALL_COMMAND $(MAKE) install
    BUILD_IN_SOURCE 1
)

# Specify the include directory and library
set(SECP256K1_INCLUDE_DIR ${SECP256K1_INSTALL_DIR}/include PARENT_SCOPE)
set(SECP256K1_LIBRARY ${SECP256K1_INSTALL_DIR}/lib/libsecp256k1.a PARENT_SCOPE)

# Add Google Test
add_subdirectory(googletest)
