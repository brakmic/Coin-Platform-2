#include <iostream>
#include "../src/cryptography/cryptography.hpp"
#include "../src/common/utilities.hpp"

int main()
{
  // Generate a new private key
  PrivateKey private_key = cryptography::generate_private_key();

  // Convert the private key to a hexadecimal string
  std::string private_key_hex = utilities::bytes_to_hex(bytes(private_key.begin(), private_key.end()));

  // Output the generated private key
  std::cout << "Generated Private Key: " << private_key_hex << std::endl;

  return 0;
}
